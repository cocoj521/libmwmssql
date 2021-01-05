// SafeLock.cpp: implementation of the SafeLock class.
//
//////////////////////////////////////////////////////////////////////

#include "SafeLock.h"
#include "LockBase.h"
#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SafeLock::SafeLock()
{
    m_pLock = NULL;
}

SafeLock::SafeLock(LockBase* pLock)
{
    m_pLock = pLock;
    if (m_pLock)
    {
        m_pLock->Lock();
    }
}

SafeLock::~SafeLock()
{
    if (m_pLock)
    {
        m_pLock->UnLock();
        m_pLock = NULL;
    }
}
