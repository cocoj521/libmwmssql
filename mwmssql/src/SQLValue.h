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

//×Ö¶ÎÖµ
class SQLVALUE
{
public:
	SQLVALUE() {};
	virtual ~SQLVALUE() {};
public:
	SQLVALUE& operator=(const SQLVALUE& other)
	{
		if (this == &other)
		{
			return *this;
		}
		value = other.value;
		return *this;
	}

	inline operator const char*() const
	{
		return (const char*)value;
	}

	inline operator char() const
	{
		return (char)value;
	}

	inline operator short() const
	{
		return (short)value;
	}

	inline operator int() const
	{
		return (int)value;
	}

	inline operator long() const
	{
		return (long)value;
	}

	inline operator __int64() const
	{
		return (__int64)value;
	}

	inline operator float() const
	{
		return (float)value;
	}

	inline operator double() const
	{
		return (double)value;
	}

	inline unsigned long GetLength() const
	{
		return value.GetLength();
	}

	/*inline SQLType::e_Type GetType() const
	{
	return value.GetType();
	}*/

	inline bool IsNull() const
	{
		return value.IsNull();
	}

	/*inline bool IsBLOB() const
	{
	return value.IsBLOB();
	}*/

	inline const char* GetData() const
	{
		return value.GetData();
	}
public:
	CSQLValue value;
};
}

#endif
