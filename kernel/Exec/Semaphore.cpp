#define DEBUGME
#undef DEBUGME

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
  if (mSharedCount || (mOwner && mOwner != gExecBase.GetCurrentTask())) {
    // shouldn't happen!
    gExecBase.GuruMeditation("Semaphore (%s) destructor called while in use\n", NodeName());
  }
}

TBool Semaphore::Attempt(TBool aExclusive) {
  BTask *t = gExecBase.GetCurrentTask();
  if (t == mOwner) {
    // nested attempts
    DSPACE();
    DLOG("Semaphore::Attempt nested succeeded\n");
    mNestCount++;
    return ETrue;
  }

  if (mOwner) {
    // someone else has the lock
    DSPACE();
    DLOG("Semaphore::Attempt Semaphore in use mOwner (%s)\n", mOwner->TaskName());
    return EFalse;
  }
  else if (aExclusive) {
    if (mSharedCount) {
      // one or more tasks have a shared lock
      DSPACE();
      DLOG("Semaphore::Attempt Shared Semaphore in use\n");
      return EFalse;
    }
    // grab the lock, it's ours!
    DSPACE();
    DLOG("Semaphore::Attempt Smaphore success - obtained by(%s)\n", t->TaskName());
    mOwner = t;
    mNestCount = 1;
    mSharedCount = 0;
    return ETrue;
  }
  else {
    // attempt shared lock
    DSPACE();
    DLOG("Semaphore::Attempt shared lock success\n");
    mSharedCount++;
    return ETrue;
  }
}

void Semaphore::Obtain(TBool aExclusive) {
  BTask *t = gExecBase.GetCurrentTask();

  if (Attempt(aExclusive)) {
    return;
  }
  DLOG("Semaphore in use (about to block) currentTask(%s) this(%x)\n", t->TaskName(), this);

  gExecBase.WaitSemaphore(t, this);
  // t->Remove();
  // mWaitingCount++;
  // mWaitingTasks->AddTail(*t);
  // Dump();
  // gExecBase.mCurrentTask = ENull;
  // gExecBase.Schedule();
  // // gExecBase.WaitSemaphore(t, this);
  DLOG("%s got semapnore\n", t->TaskName());
}

TBool Semaphore::Release() {
  DSPACE();
  DLOG("Semaphore::Release()\n");

  BTask *t = gExecBase.GetCurrentTask();
  if (mOwner == t) {
    // release the exclusive lock
    DLOG("Release exclusive lock\n");
    gExecBase.ReleaseSemaphore(this);
    DLOG("RELEASED (%s)\n", gExecBase.CurrentTaskName());

    return ETrue;
  }
  else if (mSharedCount > 0) {
    DLOG("Release shared lock\n");
    mSharedCount--;
    if (mSharedCount <= 0) {
      // wake up first task that's waiting for the lock
      gExecBase.ReleaseSemaphore(this);
    }
    DLOG("RELEADED SHARED LOCK\n");
    return ETrue;
  }
  DLOG("*** can't unlock Semaphore not owned by (%s)\n", t->TaskName());
  return EFalse;
}
