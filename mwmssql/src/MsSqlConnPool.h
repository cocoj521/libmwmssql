// MsSqlConnPool.h: interface for the CMsSqlConnPool class and CMsSqlConnPoolMgr class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MSSQLCONNPOOL_H__
#define __MSSQLCONNPOOL_H__

#include "SafeLock.h"
#include "DerivedLock.h"
#include <string>
#include <string.h>
#include <list>
#include <map>
#include "MsSqlOperator.h"

namespace MsSqlConnPool
{

using namespace MSSQL_SQLBASE_NSAMESPACE;
using namespace MSSQL_SQLTYPES_NSAMESPACE;
using namespace MSSQL_SQLCONNECTION_NSAMESPACE;
using namespace MSSQL_SQLVALUE_NSAMESPACE;
using namespace MSSQL_SQLRESULT_NSAMESPACE;
using namespace MSSQL_SQLEXCEPTION_NSAMESPACE;
using namespace MSSQL_SQLFIELD_NSAMESPACE;
using namespace MSSQL_SQLRESULT_NSAMESPACE;
using namespace MsSqlOperator;

#define MAX_DBCONNPOOL_SIZE		5					//最大可创建的连接池数
#define MIN_DBCONN_SIZE			1					//每个连接池允许创建的最小连接数
#define	DEFAUT_DBCONN_SIZE		5					//每个连接池默认创建的连接数
#define MAX_DBCONN_SIZE			255					//每个连接池允许创建的最大连接数
#define MAX_DBCONN_ACTIVE_TM	30					//连接激活时间
#define MAX_DBCONN_RECON_TM		3					//重连
#define DEFAULT_POOLNAME		"DEFPOOL"			//默认连接池名称
#define MIN_WAITCONN_TM			5					//等待获取连接的超时值单位秒

typedef  long* PLONG;

struct DBCONN 
{
	DBConnection* pConn;

	time_t tLastActive; //上次使用时间
	
	DBCONN()
    {
        pConn = NULL;
		time(&tLastActive);
    }
	
    DBCONN& operator=(const DBCONN& other)
    {
        if (this == &other)
        {
            return *this;
        }
        memset(this, 0, sizeof(DBCONN));
        memcpy(this, &other, sizeof(DBCONN));
        return *this;
    }
};

//数据库连接池类
class CMsSqlConnPool
{
	friend class CMsSqlConnPoolMgr;

public:
    CMsSqlConnPool();
    virtual ~CMsSqlConnPool();
private:  
	//初始化连接池,并申请nMinSize个连接,当所有连接都申请成功时返回true.
	bool InitPool(std::string& strErrMsg, std::string& strErrDesc, const char* szDSName, const char* szUserName, const char* szPassWord, const char* szActiveSql="SELECT 1"/*激活语句*/, int nTimeOut=60, int nMinSize=MIN_DBCONN_SIZE, int nMaxSize=MAX_DBCONN_SIZE);
	
	//从池子中获取一个连接.可指定最大等待时间(单位:s),超时将自动返回NULL.最大等待时间最小值为5s
	DBConnection* GetConnFromPool(int nMaxTimeWait=MIN_WAITCONN_TM);
	
	//回收连接
	bool RecycleConn(DBConnection* pConn);
	
	//需要检测连接是否还有效
	bool IsConnValid(DBConnection* pConn);
	
	//获取连接池信息
	void GetConnPoolInfo(int& nMinSize, int& nMaxSize, int& nIdle, int& nBusy);
	
	//重置连接池大小
	void ResetConnPoolSize(int nMinSize, int nMaxSize);
private:
	//测试连接是否可用
	bool TestConn(DBConnection* pConn);
	//销毁连接池,并释放所有连接
	void UnInitPool();
	//申请新连接
	bool ApplayNewConn(DBCONN& conn, std::string& strErrMsg, std::string& strErrDesc);
	//激活长时间空闲的连接
	void ActiveIdleConn();
private:
	//对断线的连接进行重试连接
	void CheckOFFLineDBCon();

	//把已确认Ok的连接加入空闲LIST
	void AddIdleList(DBCONN dbcon);

	//把确认为断开的连接加入重连LIST
	void AddRecyleList(DBCONN dbcon);

	//一次性取出所有的断开的连接
	void GetAllOFFlineCon(std::list<DBCONN>& listOffLine);

private:
	//连接使用完成后回收工作线程
	DerivedLock m_csRecyleLock;

	//使用后的连接进入回收缓冲
	std::list<DBCONN> m_LRecyle;
private:
	std::string m_strDSName;
	std::string m_strUserName;
	std::string m_strPassWord;
	int m_nTimeOut;
	int m_nMinSize;
	int m_nMaxSize;
	std::string m_strActiveSql;
	std::map<DBConnection*, int> m_InUse;
	std::list<DBCONN> m_Idle;
	DerivedLock m_csLock;
};

//数据库连接池管理类
class CMsSqlConnPoolMgr
{
public:
    static CMsSqlConnPoolMgr& GetInstance();//单件实例获取接口
    virtual ~CMsSqlConnPoolMgr();
public: 
	//创建连接池并指定池子的名称,池子名称区分大小写
	bool CreateConnPool(std::string& strErrMsg, std::string& strErrDesc, const char* szPoolName, const char* szDSName, const char* szUserName, const char* szPassWord, const char* szActiveSql="SELECT 1"/*激活语句*/, int nTimeOut=60, int nMinSize=5, int nMaxSize=50);
	//从指定的池子中获取一个连接.可指定最大等待时间(单位:s),超时将自动返回NULL.最大等待时间最小值为5s
	DBConnection * GetConnFromPool(const char* szPoolName, int nMaxTimeWait = 5);
	//将连接回收至指定池子中
	void RecycleConn(const char* szPoolName, DBConnection* pConn);
	//需要检测连接是否还有效
	bool IsConnValid(const char* szPoolName, DBConnection* pConn);
	//激活连接池中的空闲连接
	void ActiveIdleConn();
	//获取连接池信息
	void GetConnPoolInfo(const char* szPoolName, int& nMinSize, int& nMaxSize, int& nIdle, int& nBusy);
	//重置连接池大小
	void ResetConnPoolSize(const char* szPoolName, int nMinSize, int nMaxSize);
	//把断开的连接重新尝试重连
	void ReConnect();

	//原子加1；
	static long InterlockedIncrement(PLONG  Addend);
	
	//原子减1
	static long InterlockedDecrement(PLONG  Addend);

private:
    CMsSqlConnPoolMgr();
private:
	static void* ThreadCheckConnPool(void* pParam);
	std::map<std::string, CMsSqlConnPool*> m_dbpool;
	DerivedLock m_csLock;
	bool m_bExit;

	//线程计数器
	long m_nThrCnt;
};

//数据库连接池包装类,实现连接对象自释放
class SafeConnPool
{
public:
	//连接是否有效.如果连接用效,但执行语句去报错的话,那说明SQL语句有问题
	bool IsConnValid()
	{
		return (NULL != m_pConn)?CMsSqlConnPoolMgr::GetInstance().IsConnValid(m_strPoolName.c_str(), m_pConn):false;
	}
	//从连接池中取一个连接,这里会循环获取,至到取到为止
	DBConnection* GetConn(const char* szPoolName, int nMaxTimeWait = 5)
	{
		while (NULL == (m_pConn = CMsSqlConnPoolMgr::GetInstance().GetConnFromPool(szPoolName, nMaxTimeWait)));
		//m_pConn = CDBConnPoolMgr::GetInstance().GetConnFromPool(szPoolName, nMaxTimeWait);
		m_strPoolName = szPoolName;
		return m_pConn;
	}
	//从连接池中取一个连接
	DBConnection* GetConnEx(const char* szPoolName, int nMaxTimeWait = 5)
	{
		//while (NULL == (m_pConn = CDBConnPoolMgr::GetInstance().GetConnFromPool(szPoolName, nMaxTimeWait)));
		m_pConn = CMsSqlConnPoolMgr::GetInstance().GetConnFromPool(szPoolName, nMaxTimeWait);
		m_strPoolName = szPoolName;
		return m_pConn;
	}
public:
	SafeConnPool()
	{
		m_strPoolName = "";
		m_pConn = NULL;
	}
	virtual ~SafeConnPool()
	{
		CMsSqlConnPoolMgr::GetInstance().RecycleConn(m_strPoolName.c_str(), m_pConn);
		m_pConn = NULL;
		m_strPoolName = "";
	}
private:
	std::string m_strPoolName;
	DBConnection* m_pConn;
};
}
#endif 
