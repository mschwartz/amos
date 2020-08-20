#define DEBUGME
// #undef DEBUGME

#define USE_SPINLOCK
#undef USE_SPINLOCK

#include <Exec/ExecBase.hpp>
#include <Exec/Mutex.hpp>

Mutex::Mutex() : BBase() {
  mLock = 0;
}

Mutex::~Mutex() {
  //
}

extern "C" void spinlock(TAny *ptr);
extern "C" void spinunlock(TAny *ptr);

TBool Mutex::Try() {
  return __sync_bool_compare_and_swap(&mLock, 0, 1);
}

void Mutex::Acquire(const char *aMessage) {
  DISABLE;
#ifdef USE_SPINLOCK
  spinlock(&mLock);
#else
  while (!__sync_bool_compare_and_swap(&mLock, 0, 1)) {
    while (mLock) {
      asm("pause");
    }
  }
#ifdef DEBUGME
  if (aMessage) {
    dprint("%d Acquired %s\n", gExecBase.ProcessorId(), aMessage);
  }
#endif
#endif
  ENABLE;
}

void Mutex::Release(const char *aMessage) {
  DISABLE;
#ifdef DEBUGME
  if (aMessage) {
    dprint("%d Release %s\n", gExecBase.ProcessorId(), aMessage);
  }
#endif
#ifdef USE_SPINLOCK
  spinunlock(&mLock);
#else
  mLock = 0;
#endif
  ENABLE;
}
