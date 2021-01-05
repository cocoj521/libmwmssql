#ifndef __MSSQL_SQLTYPES_H__
#define __MSSQL_SQLTYPES_H__

#pragma warning (disable:4103)
#pragma warning (disable:4786)
//db2 cli headers
//#include <sqlcli1.h>
//#include <sqlcli.h>
//#include <sqlsystm.h>

//unixodbc headers
#include "sql.h"
#include "sqlext.h"
#include "sqltypes.h"
#include "sqlucode.h"

#include <string.h>
#include <map>
#include <vector>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include "stdio.h"
#include <iostream>

using namespace std;

#if !((defined  MW_WINDOWS) || (defined  WIN32))

#ifndef __int64
#define __int64 long long
#endif

#ifndef INT64
#define INT64 __int64
#endif

#endif

#ifndef SQL_MAX_ID_LENGTH
#define SQL_MAX_ID_LENGTH 128
#endif

#define MSSQL_SQLTYPES_NSAMESPACE           mssql_sqltypes_namespace
namespace MSSQL_SQLTYPES_NSAMESPACE {

typedef SQLHANDLE         ConnectionPtr;
typedef int               Type;
//typedef ResultSet*          RecordSetPtr;
//typedef Statement*          StatementPtr;
typedef void*        EnvironmentPtr;

#define MSSQL_SQLVALUE_NSAMESPACE          mssql_sqlvalue_namespace
#define MSSQL_SQLRESULT_NSAMESPACE         mssql_sqlresult_namespace
#define MSSQL_SQLFIELD_NSAMESPACE          mssql_sqlfield_namespace
#define MSSQL_SQLEXCEPTION_NSAMESPACE      mssql_sqlexception_namespace
#define MSSQL_SQLCONNECTION_NSAMESPACE     mssql_sqlconnection_namespace
#define MSSQL_SQLBASE_NSAMESPACE           mssql_sqlbase_namespace
struct MetaData  //TLV
{
	int nType;
    int nLen;
	char *pData;
};

//SQL”Ôæ‰¿‡–Õ
enum SQLSTRTYPE
{
    SQLSTRTYPE_NORMAL=0,    //SELECT INSERT DELETE UPDATE
    SQLSTRTYPE_PROC,        //CALL PROC();
    SQLSTRTYPE_UNKNOWN
};
}

#endif
