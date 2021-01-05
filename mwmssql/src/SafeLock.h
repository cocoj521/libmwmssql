// SafeLock.h: interface for the SafeLock class.
// Author: JiangHongbo
//////////////////////////////////////////////////////////////////////

#ifndef _SAFELOCK_H_
#define _SAFELOCK_H_

class LockBase;
class SafeLock
{
public:
    SafeLock();
    SafeLock(LockBase* pLock);
    virtual ~SafeLock();
private:
    LockBase *m_pLock;
};

#endif
