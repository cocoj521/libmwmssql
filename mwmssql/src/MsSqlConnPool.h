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

#define MAX_DBCONNPOOL_SIZE		5					//���ɴ��������ӳ���
#define MIN_DBCONN_SIZE			1					//ÿ�����ӳ�����������С������
#define	DEFAUT_DBCONN_SIZE		5					//ÿ�����ӳ�Ĭ�ϴ�����������
#define MAX_DBCONN_SIZE			255					//ÿ�����ӳ������������������
#define MAX_DBCONN_ACTIVE_TM	30					//���Ӽ���ʱ��
#define MAX_DBCONN_RECON_TM		3					//����
#define DEFAULT_POOLNAME		"DEFPOOL"			//Ĭ�����ӳ�����
#define MIN_WAITCONN_TM			5					//�ȴ���ȡ���ӵĳ�ʱֵ��λ��

typedef  long* PLONG;

struct DBCONN 
{
	DBConnection* pConn;

	time_t tLastActive; //�ϴ�ʹ��ʱ��
	
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

//���ݿ����ӳ���
class CMsSqlConnPool
{
	friend class CMsSqlConnPoolMgr;

public:
    CMsSqlConnPool();
    virtual ~CMsSqlConnPool();
private:  
	//��ʼ�����ӳ�,������nMinSize������,���������Ӷ�����ɹ�ʱ����true.
	bool InitPool(std::string& strErrMsg, std::string& strErrDesc, const char* szDSName, const char* szUserName, const char* szPassWord, const char* szActiveSql="SELECT 1"/*�������*/, int nTimeOut=60, int nMinSize=MIN_DBCONN_SIZE, int nMaxSize=MAX_DBCONN_SIZE);
	
	//�ӳ����л�ȡһ������.��ָ�����ȴ�ʱ��(��λ:s),��ʱ���Զ�����NULL.���ȴ�ʱ����СֵΪ5s
	DBConnection* GetConnFromPool(int nMaxTimeWait=MIN_WAITCONN_TM);
	
	//��������
	bool RecycleConn(DBConnection* pConn);
	
	//��Ҫ��������Ƿ���Ч
	bool IsConnValid(DBConnection* pConn);
	
	//��ȡ���ӳ���Ϣ
	void GetConnPoolInfo(int& nMinSize, int& nMaxSize, int& nIdle, int& nBusy);
	
	//�������ӳش�С
	void ResetConnPoolSize(int nMinSize, int nMaxSize);
private:
	//���������Ƿ����
	bool TestConn(DBConnection* pConn);
	//�������ӳ�,���ͷ���������
	void UnInitPool();
	//����������
	bool ApplayNewConn(DBCONN& conn, std::string& strErrMsg, std::string& strErrDesc);
	//���ʱ����е�����
	void ActiveIdleConn();
private:
	//�Զ��ߵ����ӽ�����������
	void CheckOFFLineDBCon();

	//����ȷ��Ok�����Ӽ������LIST
	void AddIdleList(DBCONN dbcon);

	//��ȷ��Ϊ�Ͽ������Ӽ�������LIST
	void AddRecyleList(DBCONN dbcon);

	//һ����ȡ�����еĶϿ�������
	void GetAllOFFlineCon(std::list<DBCONN>& listOffLine);

private:
	//����ʹ����ɺ���չ����߳�
	DerivedLock m_csRecyleLock;

	//ʹ�ú�����ӽ�����ջ���
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

//���ݿ����ӳع�����
class CMsSqlConnPoolMgr
{
public:
    static CMsSqlConnPoolMgr& GetInstance();//����ʵ����ȡ�ӿ�
    virtual ~CMsSqlConnPoolMgr();
public: 
	//�������ӳز�ָ�����ӵ�����,�����������ִ�Сд
	bool CreateConnPool(std::string& strErrMsg, std::string& strErrDesc, const char* szPoolName, const char* szDSName, const char* szUserName, const char* szPassWord, const char* szActiveSql="SELECT 1"/*�������*/, int nTimeOut=60, int nMinSize=5, int nMaxSize=50);
	//��ָ���ĳ����л�ȡһ������.��ָ�����ȴ�ʱ��(��λ:s),��ʱ���Զ�����NULL.���ȴ�ʱ����СֵΪ5s
	DBConnection * GetConnFromPool(const char* szPoolName, int nMaxTimeWait = 5);
	//�����ӻ�����ָ��������
	void RecycleConn(const char* szPoolName, DBConnection* pConn);
	//��Ҫ��������Ƿ���Ч
	bool IsConnValid(const char* szPoolName, DBConnection* pConn);
	//�������ӳ��еĿ�������
	void ActiveIdleConn();
	//��ȡ���ӳ���Ϣ
	void GetConnPoolInfo(const char* szPoolName, int& nMinSize, int& nMaxSize, int& nIdle, int& nBusy);
	//�������ӳش�С
	void ResetConnPoolSize(const char* szPoolName, int nMinSize, int nMaxSize);
	//�ѶϿ����������³�������
	void ReConnect();

	//ԭ�Ӽ�1��
	static long InterlockedIncrement(PLONG  Addend);
	
	//ԭ�Ӽ�1
	static long InterlockedDecrement(PLONG  Addend);

private:
    CMsSqlConnPoolMgr();
private:
	static void* ThreadCheckConnPool(void* pParam);
	std::map<std::string, CMsSqlConnPool*> m_dbpool;
	DerivedLock m_csLock;
	bool m_bExit;

	//�̼߳�����
	long m_nThrCnt;
};

//���ݿ����ӳذ�װ��,ʵ�����Ӷ������ͷ�
class SafeConnPool
{
public:
	//�����Ƿ���Ч.���������Ч,��ִ�����ȥ����Ļ�,��˵��SQL���������
	bool IsConnValid()
	{
		return (NULL != m_pConn)?CMsSqlConnPoolMgr::GetInstance().IsConnValid(m_strPoolName.c_str(), m_pConn):false;
	}
	//�����ӳ���ȡһ������,�����ѭ����ȡ,����ȡ��Ϊֹ
	DBConnection* GetConn(const char* szPoolName, int nMaxTimeWait = 5)
	{
		while (NULL == (m_pConn = CMsSqlConnPoolMgr::GetInstance().GetConnFromPool(szPoolName, nMaxTimeWait)));
		//m_pConn = CDBConnPoolMgr::GetInstance().GetConnFromPool(szPoolName, nMaxTimeWait);
		m_strPoolName = szPoolName;
		return m_pConn;
	}
	//�����ӳ���ȡһ������
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
