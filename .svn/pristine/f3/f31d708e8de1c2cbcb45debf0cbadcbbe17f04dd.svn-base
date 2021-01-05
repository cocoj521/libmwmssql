#include "SQLField.h"

MSSQL_SQLFIELD_NSAMESPACE::CSQLField::CSQLField(const std::string& strFieldName, int nIndex)
{
    m_strName = strFieldName;
    m_nIndex  = nIndex;
}

MSSQL_SQLFIELD_NSAMESPACE::CSQLField::CSQLField()
{

}

MSSQL_SQLFIELD_NSAMESPACE::CSQLField::CSQLField(const MSSQL_SQLFIELD_NSAMESPACE::CSQLField& rField)
{
    Copy(rField);
}

void MSSQL_SQLFIELD_NSAMESPACE::CSQLField::Copy(const MSSQL_SQLFIELD_NSAMESPACE::CSQLField& rField)
{
    m_strName = rField.m_strName;
    m_nIndex  = rField.m_nIndex;
}

MSSQL_SQLFIELD_NSAMESPACE::CSQLField& MSSQL_SQLFIELD_NSAMESPACE::CSQLField::operator=(const MSSQL_SQLFIELD_NSAMESPACE::CSQLField& rField)
{
    if(this != &rField)
    {
        Copy(rField);
    }
    return *this;
}
