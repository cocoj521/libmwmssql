// DerivedLock.h: interface for the DerivedLock class.
// Author: JiangHongbo
//////////////////////////////////////////////////////////////////////
#ifndef _DERIVEDLOCK_H_
#define _DERIVEDLOCK_H_

#include "LockBase.h"
#include <pthread.h>

typedef pthread_mutex_t CRITICAL_SECTION;

class DerivedLock : public LockBase
{
public:
    DerivedLock();
    virtual ~DerivedLock();
    virtual void Lock();
    virtual void UnLock();
    virtual void Destroy();
private:
    CRITICAL_SECTION m_cs;
};

#endif
