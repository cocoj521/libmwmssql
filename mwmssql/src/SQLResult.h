/******************************************************************************
   File:        SQLResult.h
   Contents:    CSQLResult class definition
   Description: Used to perform SQL queries and fetch the result set rows
******************************************************************************/

#ifndef __MSSQL_SQLRESULT_H__
#define __MSSQL_SQLRESULT_H__

#include "SQLBase.h"
#include "SQLValue.h"
#include "SQLField.h"

namespace MSSQL_SQLRESULT_NSAMESPACE {
//using namespace MSSQL_SQLTYPES_NSAMESPACE;
class CSQLResult : public MSSQL_SQLBASE_NSAMESPACE::CSQLBase
{
public:
   CSQLResult(MSSQL_SQLTYPES_NSAMESPACE::ConnectionPtr pCon);
   void Init(MSSQL_SQLTYPES_NSAMESPACE::ConnectionPtr pCon,MSSQL_SQLTYPES_NSAMESPACE::EnvironmentPtr pEnv);
   CSQLResult();
   virtual ~CSQLResult();
protected:
   MSSQL_SQLFIELD_NSAMESPACE::SQLFieldArray m_SQLFIELD;
   MSSQL_SQLTYPES_NSAMESPACE::ConnectionPtr m_hStmt;
   std::vector<MSSQL_SQLTYPES_NSAMESPACE::MetaData> m_vMetaData;
public:
   bool IsEmpty()
   {
       return NULL==m_hStmt;
   };
   int  GetItemIndex(const char* szField);
   int  GetAffectedRowCnt() const;
   //nType 0:��ʾszSql����ͨSQL���(select,insert,update,delete) 1:��ʾszSql�Ǵ洢����
   //nNeedRetSet��ʾszSql�Ƿ���Ҫ���ؼ�¼��
   int  Query(const char* szSql, MSSQL_SQLTYPES_NSAMESPACE::SQLSTRTYPE nType=MSSQL_SQLTYPES_NSAMESPACE::SQLSTRTYPE_NORMAL, bool bNeedRetSet=true, int timeout=60);
   bool Fetch(MSSQL_SQLVALUE_NSAMESPACE::SQLValueArray& arrValues);
   void EndFetch();
   //�������������±�Ķ�Ӧ��ϵ
   const MSSQL_SQLFIELD_NSAMESPACE::SQLFieldArray& GetColumnMap();
protected:
   unsigned int GetFieldsCount() const;
   void GetFields();
   void ThrowExceptionIfNoResultSet() const;

    int MIN2(int a, int b)
    {
        return a>b ? b : a;
    }
    int MAX2(int a, int b)
    {
        return a>b ? a : b;
    }
    void to_upper(char *str)
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
private:
	SQLLEN m_nAffectedRowCnt;
};
}
#endif
