#include <Exec/ExecBase.hpp>
#include <Exec/Mutex.hpp>

Mutex::Mutex() : BBase() {
  mLock = 0;
  mTask = ENull;
}

Mutex::~Mutex() {
  //
}

TBool Mutex::Try() {
  return __sync_bool_compare_and_swap(&mLock, 0, 1);
}

void Mutex::Acquire() {
  DISABLE;
  BTask *t = gExecBase.GetCurrentTask();
  if (mTask && mTask == t) {
    // bochs;
    // dprint("%s\n", mTask->TaskName());
    bochs;
    ENABLE;
    // return;
  }
  while (!__sync_bool_compare_and_swap(&mLock, 0, 1)) {
    asm("pause");
  }
  mTask = t;
  ENABLE;
}

void Mutex::Release() {
  mTask = ENull;
  mLock = 0;
}
