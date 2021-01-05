#include "SQLConnection.h"
#include "SQLException.h"

MSSQL_SQLCONNECTION_NSAMESPACE::CSQLConnection::CSQLConnection()
   : MSSQL_SQLBASE_NSAMESPACE::CSQLBase(NULL),m_hEnv(NULL)//,m_hStmt(NULL)
{

}

MSSQL_SQLCONNECTION_NSAMESPACE::CSQLConnection::~CSQLConnection()
{
   Close();
}

bool MSSQL_SQLCONNECTION_NSAMESPACE::CSQLConnection::Open(
														const char* szDSName,
														const char* szUserName,
														const char* szPassWord,
														unsigned int nTimeOut
														)
{
    bool bRet = true;
    try
    {
        Close();
        //alloc environment handle
        SQLRETURN nRet = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
        if (nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO)
        {
            bRet = false;
            HandleError(SQL_HANDLE_ENV, m_hEnv);
        }
        else
        {
            //Set environment Attribute to SQL_ATTR_ODBC_VERSION
			nRet = SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
            if (nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO)
            {
                bRet = false;
                HandleError(SQL_HANDLE_ENV, m_hEnv);
            }
            else
            {
                //allocate a connection handle
                nRet = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hCon);
                if (nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO)
                {
                    bRet = false;
                    HandleError(SQL_HANDLE_DBC, m_hCon);
                }
                else
                {
					// 设置登陆超时时间
					nRet = SQLSetConnectAttr(m_hCon, SQL_LOGIN_TIMEOUT, reinterpret_cast<SQLPOINTER>(nTimeOut), SQL_IS_INTEGER);
                    //connect to database
					nRet = SQLConnect(m_hCon, (SQLCHAR*)szDSName, SQL_NTS, (SQLCHAR*)szUserName, SQL_NTS, (SQLCHAR*)szPassWord, SQL_NTS);
                    if (nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO)
                    {
                        bRet = false;
                        HandleError(SQL_HANDLE_DBC, m_hCon);
                    }
                }
            }
        }
    }
    catch (exception& e)
    {
        bRet = false;
        ThrowException(0, e.what());
    }
	catch (MSSQL_SQLEXCEPTION_NSAMESPACE::CSQLException& e)
	{
		bRet = false;
		ThrowException(0, e.Description());
	}
	catch (...)
	{
		bRet = false;
		ThrowException(0, "Other");
	}
	if (!bRet)
	{
		Close();
	}
    return bRet;
}

void MSSQL_SQLCONNECTION_NSAMESPACE::CSQLConnection::Close()
{
	try
	{
/*  if (NULL != m_hStmt)
    {
        SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
        m_hStmt = NULL;
    }
*/
    if (NULL != m_hCon)
    {
        SQLDisconnect(m_hCon);
        SQLFreeHandle(SQL_HANDLE_DBC, m_hCon);
        m_hCon = NULL;
    }
    if (NULL != m_hEnv)
    {
        SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
        m_hEnv = NULL;
    }
}
	catch (...)
	{
	}
}

bool MSSQL_SQLCONNECTION_NSAMESPACE::CSQLConnection::IsOpen()
{
   return (NULL != m_hCon);
}

