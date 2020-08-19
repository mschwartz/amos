#include <Exec/Mutex.hpp>

Mutex::Mutex() : BBase() {
  mLock = 0;
}

Mutex::~Mutex() {
  //
}

TBool Mutex::Try() {
  return __sync_bool_compare_and_swap(&mLock, 0, 1);
}

void Mutex::Acquire() {
  // dlog("Acquire %s\n", mName);
  // dlog("ACQUIRE\n");
  while (!__sync_bool_compare_and_swap(&mLock, 0, 1)) {
    asm("pause");
  }
}

void Mutex::Release() {
  // dlog("RELEASE\n")
  mLock = 0;
}