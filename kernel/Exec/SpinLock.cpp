#include <Exec/ExecBase.hpp>
#include <Exec/SpinLock.hpp>

SpinLock::SpinLock() : BBase() {
  mLock = 0;
  mTask = ENull;
}

SpinLock::~SpinLock() {
  //
}

TBool SpinLock::Try() {
  return __sync_bool_compare_and_swap(&mLock, 0, 1);
}

void SpinLock::Acquire() {
  TUint64 flags = GetFlags();

  cli();
  BTask *t = gExecBase.GetCurrentTask();
  if (mTask && mTask == t) {
    // bochs;
    // dprint("%s\n", mTask->TaskName());
    bochs;
    SetFlags(flags);
    // return;
  }
  while (!__sync_bool_compare_and_swap(&mLock, 0, 1)) {
    asm("pause");
  }
  mTask = t;
  mFlags = flags;
}

void SpinLock::Release() {
  cli();
  // BTask *t = gExecBase.GetCurrentTask();
  // if (mTask && mTask != t) {
  //   dprint("release %s != %s\n", mTask->TaskName(), t->TaskName());
  //   bochs;
  // }
  mTask = ENull;
  mLock = 0;
  SetFlags(mFlags);
}
