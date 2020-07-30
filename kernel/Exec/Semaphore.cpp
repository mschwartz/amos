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
  return EFalse;
}

void Semaphore::Obtain(TBool aExclusive) {
  DISABLE;
  BTask *t = gExecBase.GetCurrentTask();
  if (t == mOwner) {
    mNestCount++;
  }
  else if (aExclusive) {
    if (mSharedCount) {
      t->Remove();
      mWaitingTasks.AddTail(*t);
    }
  }
  ENABLE;
}
