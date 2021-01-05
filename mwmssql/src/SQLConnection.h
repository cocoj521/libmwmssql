#ifndef __MSSQL_SQLCONNECTION_H__
#define __MSSQL_SQLCONNECTION_H__

#include "SQLBase.h"

namespace MSSQL_SQLCONNECTION_NSAMESPACE {
//using namespace MSSQL_SQLTYPES_NSAMESPACE;
class CSQLConnection : public MSSQL_SQLBASE_NSAMESPACE::CSQLBase
{
public:
    CSQLConnection();
    virtual ~CSQLConnection();
public:
	bool Open(
			const char* szDSName,
			const char* szUserName,
			const char* szPassWord,
			unsigned int nTimeOut=60
            );

    void Close();

    bool IsOpen();

    operator MSSQL_SQLTYPES_NSAMESPACE::ConnectionPtr() const
    {
        return m_hCon;
    }
    MSSQL_SQLTYPES_NSAMESPACE::EnvironmentPtr GetEnvironment() const
    {
        return NULL;
    }
private:
    MSSQL_SQLTYPES_NSAMESPACE::ConnectionPtr m_hEnv;
    //SQLHANDLE m_hStmt;
};
}

#endif
