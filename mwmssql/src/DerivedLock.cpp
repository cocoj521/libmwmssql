// DerivedLock.cpp: implementation of the DerivedLock class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include "DerivedLock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DerivedLock::DerivedLock()
{
    pthread_mutex_init(&m_cs, NULL);
}

DerivedLock::~DerivedLock()
{
    pthread_mutex_destroy(&m_cs);
}

void DerivedLock::Lock()
{
    pthread_mutex_lock(&m_cs);
}

void DerivedLock::UnLock()
{
    pthread_mutex_unlock(&m_cs);
}

void DerivedLock::Destroy()
{
    pthread_mutex_destroy(&m_cs);
}
