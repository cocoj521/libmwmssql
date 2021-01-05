#include "MsSqlOperator.h"

using namespace MsSqlOperator;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DBConnection::DBConnection()
{
    bValid = false;
}
DBConnection::~DBConnection()
{
	Close();
}
ConnectionPtr DBConnection::GetConnection()
{
    return (ConnectionPtr)sqlconn;
}
EnvironmentPtr DBConnection::GetEnvironment() const
{
    return sqlconn.GetEnvironment();
}
bool DBConnection::IsValid() const
{
    return bValid;
}
bool DBConnection::Open(const char* szDSName, const char* szUserName, const char* szPassWord, unsigned int nTimeOut)
{
	return (bValid = sqlconn.Open(szDSName, szUserName, szPassWord, nTimeOut));
}
bool DBConnection::Close()
{
    bValid = false;
    sqlconn.Close();
    return bValid;
}

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
RecordSet::RecordSet()
{
};
RecordSet::~RecordSet()
{
	/*rs.EndFetch();*/
};
//执行后，如果需要记录集，指针将直接移至记录集的第一条记录，movefirst函数将不再做任何操作
//注意：记录集的操作是单向的，只能前向迭代，不能往回操作。当使用了movenext后，再使用movefirst是无效的,没有一点用
int RecordSet::Open(DBConnection& dbconn, const char *szSql, int nTimeOut, SQLSTRTYPE nType, bool bNeedRecordSet)
{
    rs.Init(dbconn.GetConnection(), dbconn.GetEnvironment());
	int nRet = rs.Query(szSql, nType, bNeedRecordSet, nTimeOut);
    EndOfFile = bNeedRecordSet&&TryMoveFirst();
    return nRet;
}
void RecordSet::GetValue(const char* szItemName, SQLVALUE& value)
{
    int nIndex = rs.GetItemIndex(szItemName);
    value.value = values[nIndex];
}
//不做任何操作,open函数中已完成此功能
void RecordSet::MoveFirst()
{
    //EndOfFile = !rs.Fetch(values);
}
//movenext后，将无法再movefirst
void RecordSet::MoveNext()
{
    EndOfFile = !rs.Fetch(values);
}
void RecordSet::Close()
{
    rs.EndFetch();
    EndOfFile = true;
}
//尝试移到头，并判断是否有记录集
bool RecordSet::TryMoveFirst()
{
    bool bRet = false;
    if (!rs.IsEmpty())
    {
        bRet = !rs.Fetch(values);
    }
    return bRet;
}

//获取本行记录的数据
const SQLValueArray& RecordSet::GetThisRowValue()
{
	return values;
}

//获取列名与列下标的对应关系
const SQLFieldArray& RecordSet::GetColumnMap()
{
	return rs.GetColumnMap();
}

//获取影响行数
int RecordSet::GetAffectedRowCnt() const
{
	return rs.GetAffectedRowCnt();
}
