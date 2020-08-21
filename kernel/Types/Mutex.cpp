#define DEBUGME
#undef DEBUGME

#include <Exec/ExecBase.hpp>
#include <Types/Mutex.hpp>

// #include <atomic>

Mutex::Mutex() : BBase() {
  mName = DuplicateString("");
  mTask = ENull;
  mFlags = 0;
  // locked = ATOMIC_FLAG_INIT;
}

Mutex::~Mutex() {
}

void Mutex::Acquire(const char *aMessage) {
  return;
  DISABLE;
  mFlags = GetFlags();
  BTask *task = gExecBase.GetCurrentTask();
  // if (task == mTask && mLock) {
  //   dlog("Trying to lock again %s\n", task->TaskName());
  //   bochs;
  // }
#ifdef DEBUGME
  char buf[512];
  if (task) {
    sprintf(buf, "Acquire mutex(%s) by task (%s)\n", mName, task->TaskName());
  }
  else {
    sprintf(buf, "Acquire mutex(%s)\n", mName);
    dputs_safe(buf);
  }
#endif
  // cli();
  while (__sync_lock_test_and_set(&mLock, 1))
    ;
  mTask = task;
  ENABLE;
}

void Mutex::Release(const char *aMessage) {
  return;
  DISABLE;
#ifdef DEBUGME
  char buf[512];
  BTask *task = gExecBase.GetCurrentTask();
  if (task) {
    sprintf(buf, "Release mutex(%s) by task (%s)\n", mName, task->TaskName());
  }
  else {
    sprintf(buf, "Release  mutex(%s)\n", mName);
    dputs_safe(buf);
  }
#endif
  mTask = ENull;
  __sync_lock_release(&mLock);
  ENABLE;
  // SetFlags(mFlags);
}

#if 0
void Mutex::Acquire(const char *aMessage) {
  DISABLE;
#ifdef DEBUGME
  if (aMessage) {
    dputs("acquiring ");
    dputs(" ");
    dputs(mName);
    // dputs(" ");
    // dputs(aMessage);
    dputs("\n");
  }
#endif
  while (locked.test_and_set(std::memory_order_acquire)) {
    ;
  }
  ENABLE;
#ifdef DEBUGME
  if (aMessage) {
    dputs("acquired ");
    dputs(" ");
    dputs(mName);
    // dputs(" ";
    // dputs(aMessage);
    dputs("\n");
  }
#endif
}

void Mutex::Release(const char *aMessage) {
  DISABLE;
  locked.clear(std::memory_order_release);
  ENABLE;
#ifdef DEBUGME
  if (aMessage) {
    dputs("released ");
    dputs(" ");
    dputs(mName);
    // dputs(" ");
    // dputs(aMessage);
    dputs("\n");
  }
#endif
}
#endif
#if 0
#define USE_SPINLOCK
#undef USE_SPINLOCK

#ifdef USE_SPINLOCK
extern "C" void spinlock(TAny *ptr);
extern "C" void spinunlock(TAny *ptr);
#else

// volatile int exclusion = 0;

void Mutex::Acquire(const char *aMessage) {
  while (__sync_lock_test_and_set(&mLock, 1)) {
    // Do nothing. This GCC builtin instruction
    // ensures memory barrier.
  }
#ifdef DEBUGME
  if (aMessage) {
    dprint("%d Acquired %s\n", gExecBase.ProcessorId(), aMessage);
  }
#endif
}

void Mutex::Release(const char *aMessage) {
  __sync_synchronize(); // Memory barrier.
  mLock = 0;
#ifdef DEBUGME
  if (aMessage) {
    dprint("%d Release %s\n", gExecBase.ProcessorId(), aMessage);
  }
#endif
}

// void spin_lock(volatile TUint64 *exclusion) {
//   while (__sync_lock_test_and_set(exclusion, 1)) {
//     while (*exclusion) {
//       asm("pause");
//     }
// }

// // static void inline spin_lock(TUint64 volatile *plock) {
// //   while (!__sync_bool_compare_and_swap(plock, 0, 1)) {
// //     while (*plock) {
// //       asm("pause");
// //     }
// //   }
// // }

// static void inline spin_unlock(TUint64 volatile *plock) {
//   __sync_lock_release(plock);
// }
TBool Mutex::Try() {
  return __sync_bool_compare_and_swap(&mLock, 0, 1);
}
#endif

#if 0
void Mutex::Acquire(const char *aMessage) {
  DISABLE;
#ifdef USE_SPINLOCK
  spinlock(&mLock);
#else
  spin_lock(&mLock);
  // while (!__sync_bool_compare_and_swap(&mLock, 0, 1)) {
  //   while (mLock) {
  //     asm("pause");
  //   }
  // }
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
  spin_unlock(&mLock);
  mLock = 0;
#endif
  ENABLE;
}

#endif
#endif
