#include <Exec/ExecBase.hpp>
#include <Exec/SpinLock.hpp>

SpinLock::SpinLock() : BBase() {
  mLock = 0;
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
  while (!__sync_bool_compare_and_swap(&mLock, 0, 1)) {
    asm("pause");
  }
  __sync_synchronize();
  mFlags = flags;
}

void SpinLock::Release() {
  cli();
  // BTask *t = gExecBase.GetCurrentTask();
  // if (mTask && mTask != t) {
  //   dprint("release %s != %s\n", mTask->TaskName(), t->TaskName());
  //   bochs;
  // }
  __sync_synchronize();
  mLock = 0;
  SetFlags(mFlags);
}
