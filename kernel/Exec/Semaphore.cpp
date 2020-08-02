#define DEBUGME
// #undef DEBUGME

#include <Exec/Semaphore.hpp>
#include <Exec/ExecBase.hpp>

Semaphore::Semaphore(const char *aName, TInt64 aPriority) : BNodePri(aName, aPriority) {
  //
  mNext = mPrev = ENull;
  mSharedCount = 0;
  mWaitingCount = 0;
  mOwner = ENull;
  mWaitingTasks = new BTaskList("Semaphore Task List");
}

Semaphore::~Semaphore() {
  DISABLE;
  if (mSharedCount || (mOwner && mOwner != gExecBase.GetCurrentTask())) {
    // shouldn't happen!
    gExecBase.GuruMeditation("Semaphore (%s) destructor called while in use\n", NodeName());
  }
  ENABLE;
}

TBool Semaphore::Attempt(TBool aExclusive) {
  DISABLE;

  BTask *t = gExecBase.GetCurrentTask();
  if (t == mOwner) {
    // nested attempts
    DLOG("Semaphore::Attempt nested succeeded\n");
    mNestCount++;
    ENABLE;
    return ETrue;
  }

  if (mOwner) {
    // someone else has the lock
    DLOG("Semaphore::Attempt Semaphore in use mOwner (%s)\n", mOwner->TaskName());
    ENABLE;
    return EFalse;
  }
  else if (aExclusive) {
    if (mSharedCount) {
      // one or more tasks have a shared lock
      DLOG("Semaphore::Attempt Shared Semaphore in use\n");
      ENABLE;
      return EFalse;
    }
    // grab the lock, it's ours!
    DLOG("Semaphore::Attempt Smaphore success - obtained by(%s)\n", t->TaskName());
    mOwner = t;
    mNestCount = 1;
    mSharedCount = 0;
    ENABLE;
    return ETrue;
  }
  else {
    // attempt shared lock
    DLOG("Semaphore::Attempt shared lock success\n");
    mSharedCount++;
    ENABLE;
    return ETrue;
  }
}

void Semaphore::Obtain(TBool aExclusive) {
  BTask *t = gExecBase.GetCurrentTask();

  if (!Attempt(aExclusive)) {
    DLOG("Semaphore in use (about to block) currentTask(%s) this(%x)\n", t, this);
    gExecBase.WaitSemaphore(t, this);
  }
  // else got the lock (Attempt got it)
  DLOG("%s got semapnore\n", t->TaskName());
}

TBool Semaphore::Release() {
  DLOG("Semaphore::Release()\n");
  BTask *t = gExecBase.GetCurrentTask();
  if (mOwner == t) {
    // release the exclusive lock
    DLOG("Release exclusive lock\n");
    mOwner = ENull;
    mSharedCount = 0;
    mNestCount = 0;

    // wake up first task that's waiting for the lock
    t = mWaitingTasks->First();
    if (!mWaitingTasks->End(t)) {
      DLOG("Wake waiting task DUMP(%x)\n", this);
      gExecBase.Wake(t);
    }
    else {
      DLOG("*** No tasks waiting for this semaphore\n");
    }
  }
  else if (mSharedCount) {
    mSharedCount--;
    if (mSharedCount <= 0) {
      // wake up first task that's waiting for the lock
      t = mWaitingTasks->First();
      if (!mWaitingTasks->End(t)) {
        DLOG("******** unblock Wake(%s)\n", t->TaskName());
        gExecBase.Wake(t);
      }
      else {
        DLOG("*** No tasks waiting for this semaphore\n");
      }
    }
  }
  else {
    DLOG("*** can't unlock Semaphore not owned by (%s)\n", t->TaskName());
    return EFalse;
  }
  return ETrue;
}
