#ifndef AMOS_EXEC_SEMAPHORE_HPP
#define AMOS_EXEC_SEMAPHORE_HPP

#include <Exec/BTask.hpp>

class Semaphore : public BNodePri {
  friend ExecBase;

public:
  Semaphore(const char *aName, TInt64 aPriority = 0);
  ~Semaphore();

public:
  // obtain the semaphore, shared if aExclusive is false
  void Obtain(TBool aExclusive = ETrue);
  // attempt obtain the semaphore, shared if aExclusive is false, returns ETrue if the lock was obtained (does snot put the task to sleep)
  TBool Attempt(TBool aExclusive = ETrue);
  // release the semaphore - return false if we don't have the semaphore locked
  TBool Release();

public:
  void Dump() {
    dlog("Semaphore(%s) at %x\n", mNodeName, this);
    dlog("         mOwner: %s\n", mOwner ? mOwner->TaskName() : "NONE");
    dlog("     mNestCount: %d\n", mNestCount);
    dlog("   mSharedCount: %d\n", mSharedCount);
    dlog("  mWaitingCount: %d\n", mWaitingCount);
    mWaitingTasks->Dump();
  }

protected:
  TInt32 mWaitingCount;     // number of tasks waiting
  TInt32 mNestCount;        // number of locks current task has on this semaphore
  TInt32 mSharedCount;      // number of tasks sharing this semaphore
  BTaskList *mWaitingTasks; // list of tasks waiting for this semaphore
  BTask *mOwner;            // task that owns this semaphore
};

class SemaphoreList : public BListPri {
public:
  SemaphoreList() : BListPri("SempahoreList") {
    dlog("Construct SemaphoreList\n");
  }
  ~SemaphoreList() {}

public:
  void Lock() { mMutex.Acquire(); }
  void Unlock() { mMutex.Release(); }

protected:
  Mutex mMutex;
};

#endif
