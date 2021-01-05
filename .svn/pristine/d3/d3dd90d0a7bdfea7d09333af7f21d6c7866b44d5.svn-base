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

bool MSSQL_SQLCONNECTION_NSAMESPACE::CSQLConnection::Open(const char* szHost,
                        const char* szDatabase,
                        const char* szUser,
                        const char* szPassword,
                        unsigned int nPort)
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
            //Close();
            HandleError(SQL_HANDLE_ENV, m_hEnv);
        }
        else
        {
            //Set environment Attribute to SQL_ATTR_ODBC_VERSION
            nRet = SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
            if (nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO)
            {
                bRet = false;
                //Close();
                HandleError(SQL_HANDLE_ENV, m_hEnv);
            }
            else
            {
                //allocate a connection handle
                nRet = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hCon);
                if (nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO)
                {
                    bRet = false;
                    //Close();
                    HandleError(SQL_HANDLE_DBC, m_hCon);
                }
                else
                {
                    //nRet = SQLSetConnectAttr(m_hCon, SQL_ATTR_ODBC_CURSORS, (SQLPOINTER)SQL_CUR_USE_ODBC, SQL_IS_INTEGER);
                    //connect to database
                    nRet = SQLConnect(m_hCon, (SQLCHAR*)szDatabase, SQL_NTS, (SQLCHAR*)szUser, SQL_NTS, (SQLCHAR*)szPassword, SQL_NTS);
                    if (nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO)
                    {
                        bRet = false;
                        //Close();
                        HandleError(SQL_HANDLE_DBC, m_hCon);
                    }
                    /*else
                    {
                        //Alloc Statement Handle
                        nRet = SQLAllocHandle(SQL_HANDLE_STMT, m_hCon, &m_hStmt);
                        if (nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO)
                        {
                            bRet = false;
                            Close();
                        }
                    }*/
                }
            }
        }
    }
    catch (exception& e)
    {
        bRet = false;
        ThrowException(0, e.what());
    }
    return bRet;
}

void MSSQL_SQLCONNECTION_NSAMESPACE::CSQLConnection::Close()
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

bool MSSQL_SQLCONNECTION_NSAMESPACE::CSQLConnection::IsOpen()
{
   return (NULL != m_hCon);
}

