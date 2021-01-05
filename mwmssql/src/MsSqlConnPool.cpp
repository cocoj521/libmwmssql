// MsSqlConnPool.cpp: implementation of the CMsSqlConnPool class and CMsSqlConnPoolMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "MsSqlHelper.h"
#include "MsSqlConnPool.h"
#include "unistd.h"
#include <sys/prctl.h>

//MsSqlConnPool
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////连接池尚遗留的缺点:连接激活检测部分会因为执行数据库操作,占用锁......
using namespace MsSqlConnPool;

CMsSqlConnPool::CMsSqlConnPool()
{
	m_nMinSize = 2;
	m_nMaxSize = 5;
	m_strActiveSql = "SELECT 1";
}

CMsSqlConnPool::~CMsSqlConnPool()
{
	UnInitPool();
}

//初始化连接池,并申请nMinSize个连接,当所有连接都申请成功时返回true.
bool CMsSqlConnPool::InitPool(std::string& strErrMsg, std::string& strErrDesc, const char* szDSName, const char* szUserName, const char* szPassWord, const char* szActiveSql, int nTimeOut, int nMinSize, int nMaxSize)
{
	bool bRet = true;
	m_strDSName = szDSName; m_strUserName = szUserName; m_strPassWord = szPassWord; m_nTimeOut = nTimeOut;
	if (NULL != szActiveSql && '\0' != szActiveSql[0])
	{
		m_strActiveSql = szActiveSql;
	}
	
	m_nMinSize = nMinSize;
	m_nMaxSize = nMaxSize;
	m_nMinSize<MIN_DBCONN_SIZE?m_nMinSize=MIN_DBCONN_SIZE:1;
	m_nMinSize>MAX_DBCONN_SIZE?m_nMinSize=MAX_DBCONN_SIZE:1;
	m_nMaxSize<MIN_DBCONN_SIZE?m_nMaxSize=MIN_DBCONN_SIZE:1;
	m_nMaxSize>MAX_DBCONN_SIZE?m_nMaxSize=MAX_DBCONN_SIZE:1;
	for (int i = 0; i < m_nMinSize; ++i)
	{
		DBCONN conn;
		if (ApplayNewConn(conn, strErrMsg, strErrDesc))
		{
			m_Idle.push_back(conn);
		}
		else
		{
			bRet = false;
			break;
		}
	}
	return bRet;
}

//获取连接池信息
void CMsSqlConnPool::GetConnPoolInfo(int& nMinSize, int& nMaxSize, int& nIdle, int& nBusy)
{
	SafeLock safelock(&m_csLock);
	nMinSize = m_nMinSize;
	nMaxSize = m_nMaxSize;
	nIdle    = m_Idle.size();
	nBusy	 = m_InUse.size();
}

//重置连接池大小
void CMsSqlConnPool::ResetConnPoolSize(int nMinSize, int nMaxSize)
{
	m_nMinSize = nMinSize;
	m_nMaxSize = nMaxSize;
	m_nMinSize<MIN_DBCONN_SIZE?m_nMinSize=MIN_DBCONN_SIZE:1;
	m_nMinSize>MAX_DBCONN_SIZE?m_nMinSize=MAX_DBCONN_SIZE:1;
	m_nMaxSize<MIN_DBCONN_SIZE?m_nMaxSize=MIN_DBCONN_SIZE:1;
	m_nMaxSize>MAX_DBCONN_SIZE?m_nMaxSize=MAX_DBCONN_SIZE:1;
}

//销毁连接池,并释放所有连接
void CMsSqlConnPool::UnInitPool()
{
	SafeLock safelock(&m_csLock);
	std::list<DBCONN>::iterator it = m_Idle.begin();
	for (it; it != m_Idle.end(); ++it)
	{
		delete it->pConn;
		it->pConn = NULL;
	}
	m_Idle.clear();
	std::map<DBConnection*, int>::iterator it2 = m_InUse.begin();
	for (it2; it2 != m_InUse.end(); ++it2)
	{
		delete it2->first;
	}
	m_InUse.clear();
}

//申请新连接
bool CMsSqlConnPool::ApplayNewConn(DBCONN& conn, std::string& strErrMsg, std::string& strErrDesc)
{
	bool bRet = false;
	DBConnection* pConn = NULL;
	try
	{
		pConn = conn.pConn = new DBConnection();
		if (NULL != conn.pConn)
		{
			if(conn.pConn->Open(m_strDSName.c_str(), m_strUserName.c_str(), m_strPassWord.c_str(), m_nTimeOut))
			{
				time(&conn.tLastActive);
				bRet = true;
			}
		}
	}
	catch (CSQLException e)
	{
		strErrMsg  = e.ErrorMessage();
		strErrDesc = e.Description();
	}
	catch (...)
	{
		strErrMsg = strErrDesc = "Other Error.";
	}
	if (!bRet && pConn)
	{
		delete pConn;
		pConn = NULL;
	}
	return bRet;
}


//从池子中获取一个连接.可指定最大等待时间(单位:s),超时将自动返回NULL.最大等待时间最小值为5s
DBConnection* CMsSqlConnPool::GetConnFromPool(int nMaxTimeWait)
{
	nMaxTimeWait < 5 ? nMaxTimeWait = 5 : 1;
	DBConnection *pCon = NULL;
	//看空闲队列中是否有可用的,若有则直接返回,若没有,查看是否超过池子最大值,若没超出则申请一个新的返回,若超出则等待
	SafeLock safelock(&m_csLock);
	if (!m_Idle.empty())
	{
		std::list<DBCONN>::iterator it = m_Idle.begin();
		for (it; it != m_Idle.end();)
		{
			//2013-06-28
			//取连接时不每次都active,减少数据库操作次数
			if (it->pConn->IsValid()) 
			{
				time(&it->tLastActive);
				m_InUse.insert(std::make_pair(it->pConn, 0));
				pCon = it->pConn;
				m_Idle.erase(it++);
				break;
			}
			else
			{
				it->pConn->Close();
				delete it->pConn;
				m_Idle.erase(it++);
				continue;
			}
		}
	}
	//空闲队列为空,但总连接没有超过最大值,则重新申请一个连接
	else if (m_Idle.empty() && m_InUse.size() < m_nMaxSize)
	{
		DBCONN conn;
		std::string strErrMsg;
		std::string strErrDesc;
		if (ApplayNewConn(conn, strErrMsg, strErrDesc))
		{
			pCon = conn.pConn;
			m_InUse.insert(std::make_pair(pCon, 0));
		}
	}

	return pCon;
}

//回收连接
bool CMsSqlConnPool::RecycleConn(DBConnection* pConn)
{
	bool bRes = true;
	if (NULL != pConn)
	{
		SafeLock safelock(&m_csLock);
		DBCONN conn;
		conn.pConn = pConn;
		time(&conn.tLastActive);
		//回收前检测连接是否可用,若不可用则清除,然后再判断是否小于最小值,若小于则再申请一下
		if (m_InUse.find(pConn) != m_InUse.end())
		{
			//2013-06-28
			//直接回收,不再active,减少操作次数
			if (!pConn->IsValid())
			{
				//连接不可用则删除掉
				//pConn->Close();
				//delete pConn;
				m_InUse.erase(pConn);
				bRes = false;
			}
			else
			{
				m_Idle.push_back(conn);
				m_InUse.erase(pConn);
			}
		}
	}

	return bRes;
}


//把已确认Ok的连接加入空闲LIST
void CMsSqlConnPool::AddIdleList(DBCONN dbcon)
{
	SafeLock lock(&m_csLock);
	m_Idle.push_back(dbcon);
}

//把确认为断开的连接加入重连LIST
void CMsSqlConnPool::AddRecyleList(DBCONN dbcon)
{
	SafeLock lock(&m_csRecyleLock);
	m_LRecyle.push_back(dbcon);
}

//一次性取出所有的断开的连接
void CMsSqlConnPool::GetAllOFFlineCon(std::list<DBCONN>& listOffLine)
{
	listOffLine.clear();
	SafeLock lock(&m_csRecyleLock);
	listOffLine.swap(m_LRecyle);
}

//对断线的连接进行重试连接
void CMsSqlConnPool::CheckOFFLineDBCon()
{
	//一次性取出所有断开的连接
	std::list<DBCONN> LOffLine;
	GetAllOFFlineCon(LOffLine);

	std::list<DBCONN>::iterator it ;
	for (it = LOffLine.begin();it != LOffLine.end();)
	{
		DBCONN& dbcon = *it;
		//检测当前连接能否正常使用
		if (TestConn(dbcon.pConn))
		{
			//加入返回容器
			AddIdleList(*it);

			//删除已连接上的连接
			LOffLine.erase(it++);
		}
		else
		{
			//重新加回断开容器
			AddRecyleList(*it);

			//等一次重连接检测。
			++it;
		}
	}
}


//需要检测连接是否还有效
bool CMsSqlConnPool::IsConnValid(DBConnection* pConn)
{
	return TestConn(pConn);
}

//测试连接是否可用
bool CMsSqlConnPool::TestConn(DBConnection* pConn)
{
	bool bRet = false;
	try
	{
		if (NULL != pConn)
		{
			bool bValid = pConn->IsValid();
			//连接正常
			if (bValid)
			{
				RecordSet rs;
				//激活
				if (0 == rs.Open(*pConn,m_strActiveSql.c_str(),60,SQLSTRTYPE_NORMAL,false))
				{
					bRet = true;
				}
			}
			else
			{
				bRet = pConn->Open(m_strDSName.c_str(), m_strUserName.c_str(), m_strPassWord.c_str(), m_nTimeOut);
			}
		}
	}
	catch (...)
	{
		bRet = false;
	}
	if (pConn && !bRet)
	{
		pConn->Close();
	}
	return bRet;
}

//激活长时间空闲的连接(若当前连接总数大于连接池上限，则主动关闭空闲连接)
void CMsSqlConnPool::ActiveIdleConn()
{
	SafeLock safelock(&m_csLock);
	std::list<DBCONN>::iterator it = m_Idle.begin();
	for (it; it != m_Idle.end(); )
	{
		//有空闲的并且连接总数超过了上限，则主动关闭连接
		if (m_InUse.size()+m_Idle.size() > m_nMaxSize)
		{
			it->pConn->Close();
			delete it->pConn;
			m_Idle.erase(it++);
		}
		else
		{
			//对连接进行激活
			if (time(NULL) - it->tLastActive > MAX_DBCONN_ACTIVE_TM && 0 != TestConn(it->pConn))
			{
				delete it->pConn;
				m_Idle.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}
}

///CMsSqlConnPoolMgr
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsSqlConnPoolMgr& CMsSqlConnPoolMgr::GetInstance()
{
    static CMsSqlConnPoolMgr instance;
    return instance;
}

CMsSqlConnPoolMgr::CMsSqlConnPoolMgr()
{
	m_bExit = false;
	pthread_t ntid;
	int err = pthread_create(&ntid, NULL, ThreadCheckConnPool, this);
	pthread_detach(ntid);
}

CMsSqlConnPoolMgr::~CMsSqlConnPoolMgr()
{
	//通知线程退出
	m_bExit = true;

	while (m_nThrCnt > 0)
	{
		usleep(100*1000);
	}

	SafeLock safelock(&m_csLock);
	std::map<std::string, CMsSqlConnPool*>::iterator it = m_dbpool.begin();
	for (it; it != m_dbpool.end(); ++it)
	{
		it->second->UnInitPool();
		delete it->second;
		it->second = NULL;
	}
	m_dbpool.clear();
}

//创建连接池并指定池子的名称,若使用""做为池子名称,则创建默认池名,池子名称区分大小写
bool CMsSqlConnPoolMgr::CreateConnPool(std::string& strErrMsg, std::string& strErrDesc, const char* szPoolName, const char* szDSName, const char* szUserName, const char* szPassWord, const char* szActiveSql, int nTimeOut, int nMinSize, int nMaxSize)
{
	bool bRet = false;
	CMsSqlConnPool *pPool = new CMsSqlConnPool();
	if (NULL != pPool)
	{
		if (pPool->InitPool(strErrMsg, strErrDesc, szDSName, szUserName, szPassWord, szActiveSql, nTimeOut, nMinSize, nMaxSize))
		{
			SafeLock safelock(&m_csLock);
			m_dbpool.insert(std::make_pair(szPoolName, pPool));
			bRet = true;
		}
		else
		{
			delete pPool;//AAAAAAAAAAAAAAAAA
			pPool = NULL;
		}
	}
	return bRet;
}


//从指定的池子中获取一个连接.可指定最大等待时间(单位:s),超时将自动返回NULL.最大等待时间最小值为5s
DBConnection * CMsSqlConnPoolMgr::GetConnFromPool(const char* szPoolName, int nMaxTimeWait)
{
	DBConnection* pCon = NULL;
	CMsSqlConnPool * pPool = NULL;
	{
		SafeLock safelock(&m_csLock);
		std::map<std::string, CMsSqlConnPool*>::iterator it = m_dbpool.find(szPoolName);
		if (it != m_dbpool.end())
		{
			pPool = it->second;
		}
	}
	if (NULL != pPool)
	{
		pCon = pPool->GetConnFromPool(nMaxTimeWait);
	}
	time_t tBegin = time(NULL);
	while (NULL == pCon && ((time(NULL)-tBegin) >= nMaxTimeWait || (time(NULL)-tBegin) < 0))
	{
		{
			SafeLock safelock(&m_csLock);
			std::map<std::string, CMsSqlConnPool*>::iterator it = m_dbpool.find(szPoolName);
			if (it != m_dbpool.end())
			{
				pPool = it->second;
			}
		}

		if (NULL != pPool)
		{
			pCon = pPool->GetConnFromPool(nMaxTimeWait);
			if (NULL != pCon) break;
		}
		usleep(1000);
	}

	return pCon;
}

//将连接回收至指定池子中
void CMsSqlConnPoolMgr::RecycleConn(const char* szPoolName, DBConnection* pConn)
{
	CMsSqlConnPool * pPool = NULL;
	{
		SafeLock safelock(&m_csLock);
		std::map<std::string, CMsSqlConnPool*>::iterator it = m_dbpool.find(szPoolName);
		if (it != m_dbpool.end())
		{
			pPool = it->second;
		}
	}

	if (NULL != pPool)
	{
		//回收成功则从使用中的容
		//器转移到了空闲容器之中
		if(!pPool->RecycleConn(pConn))
		{
			//回收失败则从使用中的容器转移
			//到定时重连接容器之中
			DBCONN conn;
			conn.pConn = pConn;
			time(&conn.tLastActive);
			pPool->AddRecyleList(conn);
		}
	}
}

//需要检测连接是否还有效
bool CMsSqlConnPoolMgr::IsConnValid(const char* szPoolName, DBConnection* pConn)
{
	bool bRet = false;
	CMsSqlConnPool * pPool = NULL;
	{
		SafeLock safelock(&m_csLock);
		std::map<std::string, CMsSqlConnPool*>::iterator it = m_dbpool.find(szPoolName);
		if (it != m_dbpool.end())
		{
			pPool = it->second;
		}
	}
	if (pPool)
	{
		bRet = pPool->TestConn(pConn);
		//若数据库连接断开,则延时一定时间
		if (!bRet) usleep(1000); //1ms
	}
	return bRet;
}

//获取连接池信息
void CMsSqlConnPoolMgr::GetConnPoolInfo(const char* szPoolName, int& nMinSize, int& nMaxSize, int& nIdle, int& nBusy)
{
	CMsSqlConnPool * pPool = NULL;
	{
		SafeLock safelock(&m_csLock);
		std::map<std::string, CMsSqlConnPool*>::iterator it = m_dbpool.find(szPoolName);
		if (it != m_dbpool.end())
		{
			pPool = it->second;
		}
	}
	
	if (NULL != pPool)
	{
		pPool->GetConnPoolInfo(nMinSize, nMaxSize, nIdle, nBusy);
	}
}

//重置连接池大小
void CMsSqlConnPoolMgr::ResetConnPoolSize(const char* szPoolName, int nMinSize, int nMaxSize)
{
	CMsSqlConnPool * pPool = NULL;
	{
		SafeLock safelock(&m_csLock);
		std::map<std::string, CMsSqlConnPool*>::iterator it = m_dbpool.find(szPoolName);
		if (it != m_dbpool.end())
		{
			pPool = it->second;
		}
	}

	if (NULL != pPool)
	{
		pPool->ResetConnPoolSize(nMinSize, nMaxSize);
	}
}

void CMsSqlConnPoolMgr::ActiveIdleConn()
{
	std::map<std::string, CMsSqlConnPool*>::iterator it = m_dbpool.begin();
	for (it; it != m_dbpool.end() && !m_bExit; ++it)
	{
		if (NULL != it->second) it->second->ActiveIdleConn();
	}
}

//把断开的连接重新尝试重连
void CMsSqlConnPoolMgr::ReConnect()
{
	std::map<std::string, CMsSqlConnPool*>::iterator it = m_dbpool.begin();
	for (it; it != m_dbpool.end() && !m_bExit; ++it)
	{
		if (NULL != it->second) it->second->CheckOFFLineDBCon();
	}
}

//该线程检测每个连接池中的连接是否超过一定的时间没有激活，对其进行激活
void* CMsSqlConnPoolMgr::ThreadCheckConnPool(void* pParam)
{
    CMsSqlConnPoolMgr *pMsSqConnPoolMgr = (CMsSqlConnPoolMgr *)pParam;
    if (NULL == pMsSqConnPoolMgr)
    {
        return 0;
    }

	prctl(PR_SET_NAME, "MsSqlConnCheck");
	
	InterlockedIncrement(&pMsSqConnPoolMgr->m_nThrCnt);
	
	time_t tLastActive = time(NULL);
	time_t tLastCheckOffLineTM = time(NULL);
    while (!pMsSqConnPoolMgr->m_bExit)
    {
        try
        {
			if (abs(time(NULL) - tLastActive) > MAX_DBCONN_ACTIVE_TM)
			{
				pMsSqConnPoolMgr->ActiveIdleConn();
				time(&tLastActive);
			}

			if (abs(time(NULL) - tLastCheckOffLineTM) > MAX_DBCONN_RECON_TM)
			{
				pMsSqConnPoolMgr->ReConnect();
				time(&tLastCheckOffLineTM);
			}
			sleep(1);	
        }
        catch (...)
        {
        }
    }

	InterlockedDecrement(&pMsSqConnPoolMgr->m_nThrCnt);

    return 0;
}


//**********************************
//Description: 原子操作-递增
// Parameter:  [IN] PLONG  Addend 要递增的数
// Returns:    LONG  32位有符号整数
//
//************************************
long CMsSqlConnPoolMgr::InterlockedIncrement(PLONG  Addend)
{
    //return __gnu_cxx::__exchange_and_add((_Atomic_word*)Addend, 1);
    __gnu_cxx::__atomic_add((_Atomic_word*)Addend,1);
    //__atomic_add((_Atomic_word*)Addend,1);
    return *Addend;
    //return InterlockedIncrement(Addend);
    //return __gnu_cxx::__exchange_and_add((_Atomic_word*)Addend, 1);
}

//**********************************
//Description: 原子操作-递减
// Parameter:  [IN] PLONG  Addend （long*)
// Returns:    LONG  32位有符号整数
//************************************
long CMsSqlConnPoolMgr::InterlockedDecrement(PLONG  Addend)
{
    __gnu_cxx::__atomic_add((_Atomic_word*)Addend,-1);
    //__atomic_add((_Atomic_word*)Addend,-1);
    return *Addend;
    //return InterlockedDecrement(Addend);
    //return __gnu_cxx::__exchange_and_add((_Atomic_word*)Addend, -1);
}