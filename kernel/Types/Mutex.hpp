#ifndef AMOS_KERNEL_EXEC_MUTEX_H
#define AMOS_KERNEL_EXEC_MUTEX_H

/**
 * Spinlock mutex
 */

// #include <Exec/ExecBase.hpp>
#include <Exec/BBase.hpp>
#include <Exec/x86/cpu_utils.hpp>
// #include <atomic>

class BTask;

class Mutex : public BBase {
public:
  Mutex();
  ~Mutex();

public:
  void SetName(const char *aName) {
    delete[] mName;
    mName = DuplicateString(aName);
  }

protected:
  volatile int mLock ALIGN16;
  TUint64 mFlags; // saved flags
  char *mName;
  BTask *mTask;

public:
  void Acquire(const char *aMessage = ENull);
  void Release(const char *aMessage = ENull);
};

#if 0
class Mutex : public BBase {
public:
  Mutex();
  ~Mutex();

public:
  TBool Try();
  void Acquire(const char *aMessage = ENull) { lock.lock(); }
  void Release(const char *amessage = ENull) { lock.unlock(); };

protected:
  SpinLock lock;
};

#endif

#endif
