#include "SQLException.h"
#include "SQLBase.h"

void MSSQL_SQLBASE_NSAMESPACE::CSQLBase::ThrowException(int nErCode, const char* szErr) const
{
    MSSQL_SQLEXCEPTION_NSAMESPACE::CSQLException e(nErCode, szErr);
    throw e;
}

void MSSQL_SQLBASE_NSAMESPACE::CSQLBase::ThrowExceptionIfNotOpen() const
{
    if(NULL == m_hCon)
    {
        std::string strErr = "createConnection error.";
        MSSQL_SQLEXCEPTION_NSAMESPACE::CSQLException e(0, strErr.c_str());
        throw e;
    }
}

void  MSSQL_SQLBASE_NSAMESPACE::CSQLBase::HandleError(SQLSMALLINT nType, SQLHANDLE hHandle)
{
    SQLCHAR         SqlState[SQL_SQLSTATE_SIZE+1] = {0};
    SQLCHAR         ErMsg[SQL_MAX_MESSAGE_LENGTH+1] = {0};
    SQLINTEGER      NativeError;
    SQLSMALLINT     i = 1;
    SQLSMALLINT     MsgLen = 0;
    while (SQL_SUCCESS == SQLGetDiagRec(nType, hHandle, i, SqlState, &NativeError, ErMsg, sizeof(ErMsg), &MsgLen))
    {
        ++i;
        break;
    }
    ThrowException(NativeError, (char*)ErMsg);
}
