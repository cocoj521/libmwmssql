// MsSqlHelper.h: interface for the CMsSqlHelper class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MSSQLHELPER_H__
#define __MSSQLHELPER_H__

#include "SQLTypes.h"
#include "SQLResult.h"
#include "SQLField.h"
#include "SQLValue.h"
#include <string>
#include <list>
#include <set>

namespace MsSqlHelper {

using namespace MSSQL_SQLBASE_NSAMESPACE;
using namespace MSSQL_SQLTYPES_NSAMESPACE;
using namespace MSSQL_SQLVALUE_NSAMESPACE;
using namespace MSSQL_SQLRESULT_NSAMESPACE;
using namespace MSSQL_SQLFIELD_NSAMESPACE;
using namespace MSSQL_SQLRESULT_NSAMESPACE;

typedef std::list<SQLValueArray> ROW_LIST;//行:存储行数据,每个字段就是列
typedef std::map<std::string, CSQLField> COLUMN_MAP;//列:存储列名与列值在行中的下标对应关系
typedef std::list<std::string> COLUMN_NAME_LIST;
class CMsSqlRecordSet
{
public:
    CMsSqlRecordSet();
    virtual ~CMsSqlRecordSet();
public://填充RecordSet信息用
	//设置列名与列下标对应关系
	void SetColumnMap(const COLUMN_MAP& column_map);
	//添加一行数据
	void AddOneRow(const SQLValueArray& one_row);
	//返回总的记录集数量
	size_t GetRecordsNum() const;
public://对外提供调用	
	//移至结录集的第一行记录
	void MoveFirst();
	//移至结录集的下一行记录
	void MoveNext();
	//是否已经是结尾
	bool bEOF();
	//使用列名获取当前行中该列的值
	bool GetFieldValueFromCurrRow(const std::string& strFieldName, SQLVALUE& value); 
    //按索引顺序获取所有存储列名的列表
    bool GetFieldNameList(COLUMN_NAME_LIST& fieldNameList);
private:
	int MIN2(int a, int b);
   	void to_upper(char *str);
private:
	ROW_LIST::iterator m_itCurr;
	COLUMN_MAP m_ColumnMap;
	ROW_LIST m_RowList;
};

enum EXECUTE_RESULT
{
	EXECUTE_SUCCES=0,				//成功
	EXECUTE_DBPOOL_NOT_ENOUGH,		//连接池不够用
	EXECUTE_DBCONN_DISCON,			//连接断开
	EXECUTE_EXESQL_ERROR,			//执行语句异常(语句有问题??)
	EXECUTE_OTHER_ERROR=255			//其他非DB异常(如:处理数据时异常)
};
class CMsSqlHelper
{
public:
    CMsSqlHelper();
    virtual ~CMsSqlHelper();
public:
	//创建连接池并指定池子的名称,池子名称区分大小写
	//最后一个参数szConnAttr,可以用于设置连接的特殊属性
	//返回值true表示成功,非false表示失败,具体失败原因从strErrMsg,strErrDesc中获取
	bool CreateConnPool(std::string& strErrMsg, std::string& strErrDesc, const char* szPoolName, const char* szDSName, const char* szUserName, const char* szPassWord, const char* szActiveSql="SELECT 1"/*激活语句*/, int nTimeOut=60, int nMinSize=2, int nMaxSize=5);

	//重置连接池大小
	void ReSizeConnPoolSize(const char* szPoolName, int nMinSize, int nMaxSize);
	
    //执行语句并返回结果集
    //返回值0表示成功,非0表示失败,具体失败原因从strErrMsg,strErrDesc中获取
    int ExecuteWithRecordSetRet(std::string& strErrMsg, std::string& strErrDesc, CMsSqlRecordSet& Records, const char* szPoolName, const char *szProc, int nTimeOut=60);
	
    //执行语句但不返回结果集
    //返回值0表示成功,非0表示失败,具体失败原因从strErrMsg,strErrDesc中获取
    int ExecuteWithOutRecordSetRet(std::string& strErrMsg, std::string& strErrDesc, int& nAffectedRows, const char* szPoolName, const char *szProc, int nTimeOut=60);
};
}
#endif

