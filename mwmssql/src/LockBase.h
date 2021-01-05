// LockBase.h: interface for the LockBase class.
// Author: JiangHongbo
//////////////////////////////////////////////////////////////////////

#ifndef _LOCKBASEH_
#define _LOCKBASEH_

class LockBase
{
public:
    LockBase();
    virtual ~LockBase();
    virtual void Lock() = 0;
    virtual void UnLock() = 0;
};

#endif
