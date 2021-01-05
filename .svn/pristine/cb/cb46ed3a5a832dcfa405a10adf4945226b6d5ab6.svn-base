#include "SQLException.h"
#include "SQLResult.h"
#include "SQLValue.h"
#include "SQLField.h"

MSSQL_SQLRESULT_NSAMESPACE::CSQLResult::CSQLResult(MSSQL_SQLTYPES_NSAMESPACE::ConnectionPtr pCon)
   : MSSQL_SQLBASE_NSAMESPACE::CSQLBase(pCon),m_hStmt(NULL)
{

}

MSSQL_SQLRESULT_NSAMESPACE::CSQLResult::CSQLResult():m_hStmt(NULL)
{
}


void MSSQL_SQLRESULT_NSAMESPACE::CSQLResult::Init(MSSQL_SQLTYPES_NSAMESPACE::ConnectionPtr pCon,MSSQL_SQLTYPES_NSAMESPACE::EnvironmentPtr pEnv)
{
    m_hCon    = pCon;
}

MSSQL_SQLRESULT_NSAMESPACE::CSQLResult::~CSQLResult()
{
   EndFetch();
}
/*
std::string FormatSql(const char* szProc, bool bAppendix)
{
    std::string strProc = "";
    if (NULL != szProc && '\0' != szProc[0])
    {
        int nLen = strlen(szProc);
        if ('}' == szProc[nLen-1])
        {
            --nLen;
        }
        if ('{' == szProc[0])
        {
            --nLen;
            strProc.assign(szProc + 1, nLen);
        }
        else
        {
            strProc.assign(szProc, nLen);
        }
        int nPos = 0;
        if (bAppendix)
        {
            nPos = strProc.find_last_of(')');
            if (0 != nPos)
            {
                strProc.insert(nPos, ",:1");
            }
        }
        nPos = strProc.find_first_of('(');
        if (0 != nPos)
        {
            if (',' == strProc[nPos+1])
            {
                strProc.erase(nPos+1,1);
            }
        }
    }
    return strProc;
}
*/
int MSSQL_SQLRESULT_NSAMESPACE::CSQLResult::Query(const char* szSql, MSSQL_SQLTYPES_NSAMESPACE::SQLSTRTYPE nType, bool bNeedRetSet)
{
    int nRet = -1;
    if (NULL == szSql)
    {
        return nRet;
    }

    //检查数据库是否已打开
    ThrowExceptionIfNotOpen();

    try
    {
        m_hStmt = NULL;
        /*Alloc Statement Handle*/
        SQLRETURN nSqlRet = SQLAllocHandle(SQL_HANDLE_STMT, m_hCon, &m_hStmt) ;
        if (nSqlRet != SQL_SUCCESS && nSqlRet != SQL_SUCCESS_WITH_INFO)
        {
            HandleError(SQL_HANDLE_DBC, m_hCon);
            //EndFetch();
        }
        else
        {
            if (MSSQL_SQLTYPES_NSAMESPACE::SQLSTRTYPE_NORMAL == nType)
            {
                nSqlRet = SQLSetStmtAttr(m_hStmt, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_DYNAMIC, SQL_IS_INTEGER);
            }
            //nRet = SQLSetConnectAttr(m_hCon, SQL_ATTR_ODBC_CURSORS, (SQLPOINTER)SQL_CUR_USE_ODBC, SQL_IS_INTEGER);
            //exec sql
            nSqlRet = SQLExecDirect(m_hStmt, (UCHAR*)szSql, SQL_NTS);
            if (nSqlRet == SQL_ERROR || (nSqlRet != SQL_SUCCESS && nSqlRet != SQL_SUCCESS_WITH_INFO && nSqlRet!= SQL_NO_DATA))
            {
                HandleError(SQL_HANDLE_STMT, m_hStmt);
                //EndFetch();
            }
            else
            {
                if (bNeedRetSet)
                {
                    GetFields();
                }
                nRet = 0;
            }
        }
    }
    catch (exception& e)
    {
        ThrowException(0, e.what());
    }
    return nRet;
}

bool MSSQL_SQLRESULT_NSAMESPACE::CSQLResult::Fetch(MSSQL_SQLVALUE_NSAMESPACE::SQLValueArray& arrValues, bool bFetchBLOB)
{
    arrValues.clear();
    bool bRet = false;
    ThrowExceptionIfNotOpen();
    ThrowExceptionIfNoResultSet();

    SQLRETURN nRet = SQLFetch(m_hStmt);
    if (nRet == SQL_NO_DATA)
    {
        EndFetch();
    }
    else if (nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO)
    {
        HandleError(SQL_HANDLE_STMT, m_hStmt);
        //EndFetch();
    }
    else
    {
        std::vector<MSSQL_SQLTYPES_NSAMESPACE::MetaData>::iterator it = m_vMetaData.begin();
        for (int nIndex = 0; it != m_vMetaData.end(); ++nIndex, ++it)
        {
            MSSQL_SQLVALUE_NSAMESPACE::CSQLValue value(it->pData, it->nLen, it->nType);
            arrValues.insert(std::make_pair(nIndex+1, value));
        }
        bRet = true;
    }
    return bRet;
}

void MSSQL_SQLRESULT_NSAMESPACE::CSQLResult::EndFetch()
{
    if (NULL != m_hStmt)
    {
        SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
        m_hStmt = NULL;
    }
    std::vector<MSSQL_SQLTYPES_NSAMESPACE::MetaData>::iterator it = m_vMetaData.begin();
    for (it; it != m_vMetaData.end(); ++it)
    {
        if (NULL != it->pData)
        {
            delete []it->pData;
            it->pData = NULL;
        }
    }
    m_vMetaData.clear();
}

void MSSQL_SQLRESULT_NSAMESPACE::CSQLResult::GetFields()
{
    m_SQLFIELD.clear();
    ThrowExceptionIfNotOpen();
    ThrowExceptionIfNoResultSet();

    SQLSMALLINT nColNum = 0;
    //get column num
    SQLRETURN nRet = SQLNumResultCols(m_hStmt, &nColNum);
    if (nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO)
    {
        HandleError(SQL_HANDLE_STMT, m_hStmt);
    }
    else
    {
        SQLCHAR ColName[SQL_MAX_ID_LENGTH+1] = {0};
        SQLSMALLINT ColNameLen = 0;
        SQLSMALLINT ColType = 0;
        SQLUINTEGER MaxColSize = 0;
        SQLSMALLINT DecimalDigits = 0;
        SQLINTEGER  Displaysize = 0;
        SQLINTEGER  OutLen = 0;
        MSSQL_SQLTYPES_NSAMESPACE::MetaData    md;
        //bind columns
        for (SQLSMALLINT i = 1; i <= nColNum; ++i)
        {
            //get column descriptions
            SQLDescribeCol(m_hStmt,
                i,
                ColName,
                sizeof(ColName),
                &ColNameLen,
                &ColType,
                &MaxColSize,
                &DecimalDigits,
                NULL
                );
            //get display length for column
            SQLColAttribute( m_hStmt,
                i,
                SQL_DESC_DISPLAY_SIZE,
                NULL,
                0,
                NULL,
                &Displaysize
                );
            MaxColSize = MAX2(Displaysize, strlen((char*)ColName)) + 1;
            char *p = new char[MaxColSize];
            memset(p, 0, MaxColSize);
            md.nLen  = MaxColSize;
            md.nType = SQL_C_CHAR;//ColType;
            md.pData = p;
            m_vMetaData.push_back(md);
            //bind columns to program vars, converting all types to CHAR
            SQLBindCol(m_hStmt,
                i,
                SQL_C_CHAR,
                p,
                MaxColSize,
                &OutLen
                );
            to_upper((char*)ColName);
            MSSQL_SQLFIELD_NSAMESPACE::CSQLField field((char*)ColName, i);
            m_SQLFIELD.insert(std::make_pair((char*)ColName, field));
        }
    }
}

void MSSQL_SQLRESULT_NSAMESPACE::CSQLResult::ThrowExceptionIfNoResultSet() const
{
    if(NULL == m_hStmt)
    {
        std::string strErr = "No result set,Please call Query before Fetch.";
        ThrowException(0, strErr.c_str());
    }
}

int MSSQL_SQLRESULT_NSAMESPACE::CSQLResult::GetItemIndex(const char* szField)
{
    char szBuf[64+1] = {0};
    memcpy(szBuf, szField, MIN2((int)(sizeof(szBuf)-1), (int)(strlen(szField))));
    to_upper(szBuf);
    int nIndex = m_SQLFIELD[szBuf].GetIndex();
    if (nIndex < 0)
    {
        std::string strErr;
        strErr = "\"";
        strErr += szBuf;
        strErr += "\"";
        strErr += " is not in this result set.";
        ThrowException(0, strErr.c_str());
    }
    return nIndex;
}

