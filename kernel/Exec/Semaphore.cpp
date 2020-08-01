#define DEBUGME
// #undef DEBUGME

#include <Exec/Semaphore.hpp>
#include <Exec/ExecBase.hpp>

Semaphore::Semaphore(const char *aName, TInt64 aPriority) : BNodePri(aName, aPriority) {
  //
  mNext = mPrev = ENull;
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
    DLOG("Semaphore::Attempt Nested attempts\n");
    mNestCount++;
    ENABLE;
    return ETrue;
  }
  else if (mOwner) {
    // someone else has the lock
    DLOG("Semaphore::Attempt Semaphore In Use mOwner (%s)\n", mOwner->TaskName());
    ENABLE;
    return EFalse;
  }
  else if (aExclusive) {
    if (mOwner) {
      // someone else has the lock
      DLOG("Semaphore::Attempt Semaphore In Use mOwner(%s)\n", mOwner->TaskName());
      ENABLE;
      return EFalse;
    }
    if (mSharedCount) {
      // one or more tasks have a shared lock
      DLOG("Semaphore::Attempt Shared Semaphore In Use\n");
      ENABLE;
      return EFalse;
    }
    // grab the lock, it's ours!
    DLOG("Semaphore::Attempt Smaphore obtained by(%s)\n", t->TaskName());
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
  DISABLE;
  if (!Attempt(aExclusive)) {
    BTask *t = gExecBase.GetCurrentTask();
    DLOG("Attempt failed currentTask(%s)\n", t);
    // block the task until lock is released
    // We're adding the task to our waiting tasks list in priority order
    // so high priority tasks get the lock over lower priority ones.
    t->Remove();
    mWaitingTasks.AddTail(*t);
    mWaitingCount++;
    DLOG("******** block %s, DUMP mWaitingTasks(%x)\n", t->TaskName(), this);
    mWaitingTasks.Dump();
    DLOG("Schedule (%s)\n", t->TaskName());
    gExecBase.Wake(); // this should work! :)
    ENABLE;
    DLOG("after block(%s) HALT\n", gExecBase.GetCurrentTask()->TaskName());
    mWaitingTasks.Dump();
    halt();
    // should get here after unblocked
    DLOG("after block(%s)\n", gExecBase.GetCurrentTask()->TaskName());
  }
  // else got the lock (Attempt got it)
  ENABLE;
}

TBool Semaphore::Release() {
  DISABLE;
  DLOG("Semaphore::Release()\n");
  BTask *t = gExecBase.GetCurrentTask();
  if (mOwner == t) {
    // release the exclusive lock
    DLOG("Release exclusive lock\n");
    mOwner = ENull;
    mSharedCount = 0;
    mNestCount = 0;

    // wake up first task that's waiting for the lock
    DLOG("Wake waiting task DUMP(%x)\n", this);
    t = mWaitingTasks.RemHead();
    if (t) {
      gExecBase.AddTask(t);
      mWaitingCount--;
      DLOG("Schedule (%s)\n", t->TaskName());
      gExecBase.Wake(); // this should work! :)
      ENABLE;
      mWaitingTasks.Dump();
      halt();
    }
  }
  else if (mSharedCount) {
    mSharedCount--;
    if (mSharedCount <= 0) {
      // wake up first task that's waiting for the lock
      t = mWaitingTasks.RemHead();
      if (t) {
        DLOG("******** unblock %s\n", t->TaskName());
        gExecBase.AddTask(t);
        mWaitingCount--;
        DLOG("Schedule (%s)\n", t->TaskName());
        gExecBase.Wake(); // this should work! :)
        ENABLE;
        mWaitingTasks.Dump();
        halt();
      }
      else {
        DLOG("no task waiting on semaphore\n");
      }
    }
  }
  else {
    ENABLE;
    return EFalse;
  }
  ENABLE;
  return ETrue;
}
