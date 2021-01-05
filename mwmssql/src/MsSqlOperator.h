//////////////////////////////////////////////////////////////////////
#ifndef __MSSQLOPERATOR_H__
#define __MSSQLOPERATOR_H__

#include "SQLTypes.h"
#include "SQLConnection.h"
#include "SQLException.h"
#include "SQLResult.h"
#include "SQLField.h"
#include "SQLValue.h"
#include <string>
#include <list>

namespace MsSqlOperator
{
using namespace MSSQL_SQLBASE_NSAMESPACE;
using namespace MSSQL_SQLTYPES_NSAMESPACE;
using namespace MSSQL_SQLCONNECTION_NSAMESPACE;
using namespace MSSQL_SQLVALUE_NSAMESPACE;
using namespace MSSQL_SQLRESULT_NSAMESPACE;
using namespace MSSQL_SQLEXCEPTION_NSAMESPACE;
using namespace MSSQL_SQLFIELD_NSAMESPACE;
using namespace MSSQL_SQLRESULT_NSAMESPACE;

class DBConnection
{
public:
    DBConnection();
    virtual ~DBConnection();
public:
    ConnectionPtr GetConnection();
    EnvironmentPtr GetEnvironment() const;
    bool IsValid() const;
    bool Open(const char* szDSName, const char* szUserName, const char* szPassWord, unsigned int nTimeOut=60);
    bool Close();
private:
    bool bValid;
    CSQLConnection sqlconn;
};

//结果集
class RecordSet
{
public:
    RecordSet();
    virtual ~RecordSet();
public:
    //执行后，如果需要记录集，指针将直接移至记录集的第一条记录，movefirst函数将不再做任何操作
    //注意：记录集的操作是单向的，只能前向迭代，不能往回操作。当使用了movenext后，再使用movefirst是无效的,没有一点用
    int Open(DBConnection& dbconn, const char *szSql, int nTimeOut=60, SQLSTRTYPE nType=SQLSTRTYPE_NORMAL, bool bNeedRecordSet=true);
	
    void GetValue(const char* szItemName, SQLVALUE& value);
	
    //不做任何操作,open函数中已完成此功能
    void MoveFirst();
	
    //movenext后，将无法再movefirst
    void MoveNext();
	
    void Close();

	//获取本行记录的数据
	const SQLValueArray& GetThisRowValue();

	//获取列名与列下标的对应关系
	const SQLFieldArray& GetColumnMap();

	//获取影响行数
	int	GetAffectedRowCnt() const;
private:
    //尝试移到头，并判断是否有记录集
    bool TryMoveFirst();
public:
    bool EndOfFile;
private:
    SQLValueArray values;
    CSQLResult rs;
};
}
#endif
