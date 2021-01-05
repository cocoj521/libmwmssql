#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include "SQLConnection.h"
#include "SQLException.h"
#include "SQLResult.h"
#include "SQLValue.h"
#include "MsSqlHelper.h"

using namespace MsSqlHelper;
using namespace std;

/*
using namespace MSSQL_SQLEXCEPTION_NSAMESPACE;
using namespace MSSQL_SQLCONNECTION_NSAMESPACE;
using namespace MSSQL_SQLRESULT_NSAMESPACE;
using namespace MSSQL_SQLVALUE_NSAMESPACE;
using namespace MSSQL_SQLTYPES_NSAMESPACE;

int main()
{
    CSQLConnection sqlconn;
    try
    {
        if (sqlconn.Open("db_test", "sa", "123zxcvbnm,./", 60))
        {
            CSQLResult rs(sqlconn);
            //rs.Query("select * from userfee where upper(UserId)=upper('pre001')", SQLSTRTYPE_NORMAL);
            rs.Query("select * from phone_table", SQLSTRTYPE_NORMAL, true);
            SQLValueArray values;
            CSQLValue value;
            while (rs.Fetch(values))
            {
              value = values[rs.GetItemIndex("phone")];
              printf("phone=%s\r\n", (const char*)value);
			  value = values[rs.GetItemIndex("name")];
			  printf("name=%s\r\n", (const char*)value);
//               value = values[rs.GetItemIndex("LOGINID")];
//               printf("LOGINID=%s,", (const char*)value);
//               value = values[rs.GetItemIndex("SPGATE")];
//               printf("SPGATE=%s,", (const char*)value);
//               value = values[rs.GetItemIndex("CPNO")];
//               printf("CPNO=%s\n", (const char*)value);
//                     value = values[rs.GetItemIndex("uid")];
//                 printf("uid=%d\n", (int)value);
//                 value = values[rs.GetItemIndex("userid")];
//                 printf("userid=%s\n", (const char*)value);
//                 value = values[rs.GetItemIndex("UserType")];
//                 printf("usertype=%d\n", (int)value);
//                 value = values[rs.GetItemIndex("UserPrivilege")];
//                 printf("userprivilege=%d\n", (int)value);
//                 value = values[rs.GetItemIndex("Status")];
//                 printf("status=%d\n", (int)value);
//                 value = values[rs.GetItemIndex("UserPassword")];
//                 printf("userpassword=%s\n", (const char*)value);
//                 value = values[rs.GetItemIndex("LoginIP")];
//                 printf("loginip=%s\n", (const char*)value);

            }
            //Sleep(1000);
        }
    }
    catch (CSQLException e)
    {
        printf("%s,%s", e.ErrorMessage(), e.Description());
        //CLogMgr::GetInstance().WriteLog(e.Description());
    }

    return 0;
}
*/


int main()
{
	string sql = "select * from phone_table";
	CMsSqlHelper mssqlhelper;
	std::string strErrMsg;
	std::string strErrDesc;
	if (mssqlhelper.CreateConnPool(strErrMsg, strErrDesc, "testpool", "db_test", "sa", "123zxcvbnm,./"))
	{
		CMsSqlRecordSet records;
		if (0 == mssqlhelper.ExecuteWithRecordSetRet(strErrMsg, strErrDesc, records, "testpool", sql.c_str()))
		{
			//std::map<std::string, CSQLField> COLUMN_MAP;//列:存储列名与列值在行中的下标对应关系
			/*
			COLUMN_NAME_LIST fieldNameList;
			if (records.GetFieldNameList(fieldNameList))
			{
			for (COLUMN_NAME_LIST::iterator item = fieldNameList.begin(); item != fieldNameList.end(); item++)
			{
			printf("column name:%s\n", item->c_str());
			}
			} */

			records.MoveFirst();
			while (!records.bEOF())
			{
				SQLVALUE value;
				//取该字段的值,有才取,没有时不要取
				if (records.GetFieldValueFromCurrRow("phone", value))
				{
					printf("phone=%s\n", (const char*)value);
				}
				if (records.GetFieldValueFromCurrRow("name", value))
				{
					printf("name=%s\n", (const char*)value);
				}
				records.MoveNext();
			}
		}
		//失败原因打印
		else
		{
			printf("%s--%s\n", strErrMsg.c_str(), strErrDesc.c_str());
		}
	}
	//失败原因打印
	else
	{
		printf("%s--%s\n", strErrMsg.c_str(), strErrDesc.c_str());
	}

	while (true)
	{
		sleep(1);
	}

	return 0;
}
