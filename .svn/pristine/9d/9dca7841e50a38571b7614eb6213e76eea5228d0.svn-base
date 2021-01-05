#ifndef __MSSQL_SQLVALUE_H__
#define __MSSQL_SQLVALUE_H__

#include "SQLTypes.h"

namespace MSSQL_SQLVALUE_NSAMESPACE {
//using namespace MSSQL_SQLTYPES_NSAMESPACE;
class CSQLValue
{
    //friend class CSQLConnection;
public:
    CSQLValue();
    CSQLValue(const char* pData, unsigned long nLength, MSSQL_SQLTYPES_NSAMESPACE::Type eType=SQL_CHAR);
    CSQLValue(const CSQLValue& rValue);
    virtual ~CSQLValue();
protected:
    char* m_pData;
    unsigned long m_nLength;
    MSSQL_SQLTYPES_NSAMESPACE::Type   m_eType;
public:
    CSQLValue& operator=(const CSQLValue& rValue);

    inline operator const char*() const
    {
        return reinterpret_cast<const char*>(m_pData);
    }

    inline operator char() const
    {
        return (char)*m_pData;
    }

    inline operator short() const
    {
        return static_cast<short>(atoi(*this));
    }

    inline operator int() const
    {
        return atoi(*this);
    }

    inline operator long() const
    {
        return atoi(*this);
    }

    inline operator __int64() const
    {
#if (defined  MW_WINDOWS) || (defined  WIN32)
        return _atoi64(*this);
#else
        return atoll(*this);
#endif
    }

    inline operator float() const
    {
        return static_cast<float>(atof(*this));
    }

    inline operator double() const
    {
        return atof(*this);
    }

    inline unsigned long GetLength() const
    {
        return m_nLength;
    }

    inline MSSQL_SQLTYPES_NSAMESPACE::Type GetType() const
    {
        return m_eType;
    }

    inline bool IsNull() const
    {
        return (NULL == m_pData);
    }

//     inline bool IsBLOB() const
//     {
//         return (OCCI_SQLT_BLOB == m_eType);
//     }

    inline const char* GetData() const
    {
        return (char*)m_pData;
    }
};

typedef std::map<int, CSQLValue> SQLValueArray;
}

#endif
