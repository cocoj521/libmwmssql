
// MsSqlHelper.h: interface for the CAccessMsSql class.
//
//////////////////////////////////////////////////////////////////////

#include "MsSqlHelper.h"
#include "MsSqlConnPool.h"
#include "MsSqlOperator.h"

using namespace MsSqlHelper; 
using namespace MsSqlConnPool;
using namespace MsSqlOperator;

//////////////////////////////////////////////////////////////////////////////////

CMsSqlRecordSet::CMsSqlRecordSet()
{

}

CMsSqlRecordSet::~CMsSqlRecordSet()
{

}

int CMsSqlRecordSet::MIN2(int a, int b)
{
	return a>b ? b : a;
}

void CMsSqlRecordSet::to_upper(char *str)
{
	int  i = 0;
	while(str[i] != '\0')
	{
		if((str[i] >= 'a') && (str[i] <= 'z'))
		{
			str[i] -= 32;
		}
		++i;
	}
}

//移至结录集的第一行记录
void CMsSqlRecordSet::MoveFirst()
{
	m_itCurr = m_RowList.begin();
}

//移至结录集的下一行记录
void CMsSqlRecordSet::MoveNext()
{
	if (m_itCurr != m_RowList.end())
	{
		++m_itCurr;
	}
}

//是否已经是结尾
bool CMsSqlRecordSet::bEOF()
{
	return (m_itCurr == m_RowList.end());
}

size_t CMsSqlRecordSet::GetRecordsNum() const
{
	return m_RowList.size();
}

//设置列名与列下标对应关系
void CMsSqlRecordSet::SetColumnMap(const COLUMN_MAP& column_map)
{
	m_ColumnMap = column_map;
}

//添加一行数据
void CMsSqlRecordSet::AddOneRow(const SQLValueArray& one_row)
{
	m_RowList.push_back(one_row);
}

//使用列名获取当前行中该列的值
bool CMsSqlRecordSet::GetFieldValueFromCurrRow(const std::string& strFieldName, SQLVALUE& value)
{
	bool bRet = false;

	char szBuf[64+1] = {0};
    memcpy(szBuf, strFieldName.c_str(), MIN2((int)(sizeof(szBuf)-1), (int)(strFieldName.length())));
    to_upper(szBuf);
	COLUMN_MAP::iterator it = m_ColumnMap.find(std::string(szBuf));
	if (it != m_ColumnMap.end())
	{
		int nIndex = it->second.GetIndex();
		SQLValueArray::iterator itValue = (*m_itCurr).find(nIndex);
		if (itValue != (*m_itCurr).end())
		{
			value.value = itValue->second;
			bRet = true;
		}
	}
	
	return bRet;
}

// 按升序排列
// Defining a lambda function to compare two pairs. It will compare two pairs using second field
bool compFunctor(std::pair<std::string, CSQLField> elem1, std::pair<std::string, CSQLField> elem2)
{
    return elem1.second.GetIndex() < elem2.second.GetIndex();
}

//按索引顺序获取所有存储列名的列表
bool CMsSqlRecordSet::GetFieldNameList(COLUMN_NAME_LIST& fieldNameList)
{
    bool bRet = false;
    if (!m_ColumnMap.empty())
    {
        // Declaring the type of Predicate that accepts 2 pairs and return a bool
        typedef bool(*Comparator)(std::pair<std::string, CSQLField>, std::pair<std::string, CSQLField>);

        // Declaring a set that will store the pairs using above comparision logic
        std::set<std::pair<std::string, CSQLField>, Comparator> setOfAccouts(
            m_ColumnMap.begin(), m_ColumnMap.end(), compFunctor);

        // Iterate over a set using range base for loop
        // It will display the items in sorted order of values
        std::set<std::pair<std::string, CSQLField>, Comparator>::iterator element;
        for (element = setOfAccouts.begin(); element != setOfAccouts.end(); element++)
        {
            //保存
            fieldNameList.push_back(element->second.GetName());
        }
        return true;
    }
    return bRet;
}


//////////////////////////////////////////////////////////////////////////////////
CMsSqlHelper::CMsSqlHelper()
{
}

CMsSqlHelper::~CMsSqlHelper()
{
    
}

//创建连接池并指定池子的名称,池子名称区分大小写
bool CMsSqlHelper::CreateConnPool(std::string& strErrMsg, std::string& strErrDesc, const char* szPoolName, const char* szDSName, const char* szUserName, const char* szPassWord, const char* szActiveSql, int nTimeOut, int nMinSize, int nMaxSize)
{
	return CMsSqlConnPoolMgr::GetInstance().CreateConnPool(strErrMsg, strErrDesc, szPoolName, szDSName, szUserName, szPassWord, szActiveSql, nTimeOut, nMinSize, nMaxSize);
}

//重置连接池大小
void CMsSqlHelper::ReSizeConnPoolSize(const char* szPoolName, int nMinSize, int nMaxSize)
{
	return CMsSqlConnPoolMgr::GetInstance().ResetConnPoolSize(szPoolName, nMinSize, nMaxSize);
}

//执行语句并返回结果集
int CMsSqlHelper::ExecuteWithRecordSetRet(std::string& strErrMsg, std::string& strErrDesc, CMsSqlRecordSet& Records, const char* szPoolName, const char *szSQL, int nTimeOut)
{
	int nRet = EXECUTE_DBCONN_DISCON;
	
	SafeConnPool connpool;

    try
    {
		DBConnection* pConn = connpool.GetConnEx(szPoolName);
		if (pConn)
		{
	        if (pConn->IsValid())
	        {
	        	RecordSet rs;
	            if (0 == rs.Open(*pConn, szSQL, nTimeOut, SQLSTRTYPE_NORMAL, true))
	            {
					//保存列名与列下标映射
					Records.SetColumnMap(rs.GetColumnMap());
					
					//..............遍历,保存
					while (!rs.EndOfFile)
					{
						Records.AddOneRow(rs.GetThisRowValue());	
						rs.MoveNext();
					}

					nRet = EXECUTE_SUCCES;
					strErrMsg = "SUCCESS";
					strErrDesc = "EXECUTE SUCCESS";
		        }
	        }
	        else
	        {
	            nRet = EXECUTE_DBCONN_DISCON;
				strErrMsg = "ERROR";
				strErrDesc = "Link has gone away.";
				
				connpool.IsConnValid();
	        }
		}
		else
		{
			nRet = EXECUTE_DBPOOL_NOT_ENOUGH;
			strErrMsg = "WARN";
			strErrDesc = "ConnPool is not enough or no available links.";
		}
    }
    catch (CSQLException e)
    {
        nRet = EXECUTE_EXESQL_ERROR;
		strErrMsg = e.ErrorMessage();
		strErrDesc = e.Description();
		
		connpool.IsConnValid();
    }
    catch (...)
    {
        nRet = EXECUTE_OTHER_ERROR;

        strErrMsg  = "ERROR";
		strErrDesc = "Other error.";
    }
	return nRet;
}

//执行语句但不返回结果集
int CMsSqlHelper::ExecuteWithOutRecordSetRet(std::string& strErrMsg, std::string& strErrDesc, int& nAffectedRows, const char* szPoolName, const char *szSQL, int nTimeOut)
{
	int nRet = EXECUTE_DBCONN_DISCON;

	SafeConnPool connpool;

    try
    {
		DBConnection* pConn = connpool.GetConnEx(szPoolName);
		if (pConn)
		{
	        if (pConn->IsValid())
	        {
	        	RecordSet rs;
	            if (0 == rs.Open(*pConn, szSQL, nTimeOut, SQLSTRTYPE_NORMAL, false))	
	            {
	            	//获取影响行数
	            	nAffectedRows = rs.GetAffectedRowCnt();
					
	            	nRet = EXECUTE_SUCCES;
					strErrMsg = "SUCCESS";
					strErrDesc = "EXECUTE SUCCESS";
	            }
	        }
	        else
	        {
	            nRet = EXECUTE_DBCONN_DISCON;
				strErrMsg = "ERROR";
				strErrDesc = "Link has gone away.";
				
				connpool.IsConnValid();
	        }
		}
		else 
		{
			nRet = EXECUTE_DBPOOL_NOT_ENOUGH;
			strErrMsg = "WARN";
			strErrDesc = "ConnPool is not enough or no available links.";
		}
    }
    catch (CSQLException e)
    {
        nRet = EXECUTE_EXESQL_ERROR;
		strErrMsg = e.ErrorMessage();
		strErrDesc = e.Description();

		connpool.IsConnValid();
    }
    catch (...)
    {
        nRet = EXECUTE_OTHER_ERROR;

        strErrMsg  = "ERROR";
		strErrDesc = "Other error.";
    }
    return nRet;
}

