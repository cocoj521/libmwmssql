#include <stdio.h>
#include "stdlib.h"
#include "SQLConnection.h"
#include "SQLException.h"
#include "SQLResult.h"

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
        if (sqlconn.Open("192.169.1.130", "mssqlsmsacc", "sa", "521521", 1433))
        {
            CSQLResult rs(sqlconn);
            //rs.Query("select * from userfee where upper(UserId)=upper('pre001')", SQLSTRTYPE_NORMAL);
            rs.Query("EXEC SyncUsrFeeInfo_B 100004,0,0,0,0", SQLSTRTYPE_PROC, true);
            SQLValueArray values;
            CSQLValue value;
            while (rs.Fetch(values))
            {
              value = values[rs.GetItemIndex("sendnum")];
              printf("sendnum=%s,", (int)value);
/*            value = values[rs.GetItemIndex("LOGINID")];
              printf("LOGINID=%s,", (const char*)value);
              value = values[rs.GetItemIndex("SPGATE")];
              printf("SPGATE=%s,", (const char*)value);
              value = values[rs.GetItemIndex("CPNO")];
              printf("CPNO=%s\n", (const char*)value);
                    value = values[rs.GetItemIndex("uid")];
                printf("uid=%d\n", (int)value);
                value = values[rs.GetItemIndex("userid")];
                printf("userid=%s\n", (const char*)value);
                value = values[rs.GetItemIndex("UserType")];
                printf("usertype=%d\n", (int)value);
                value = values[rs.GetItemIndex("UserPrivilege")];
                printf("userprivilege=%d\n", (int)value);
                value = values[rs.GetItemIndex("Status")];
                printf("status=%d\n", (int)value);
                value = values[rs.GetItemIndex("UserPassword")];
                printf("userpassword=%s\n", (const char*)value);
                value = values[rs.GetItemIndex("LoginIP")];
                printf("loginip=%s\n", (const char*)value);*/
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


