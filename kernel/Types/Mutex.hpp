#ifndef AMOS_KERNEL_EXEC_MUTEX_H
#define AMOS_KERNEL_EXEC_MUTEX_H

/**
 * Spinlock mutex
 */

// #include <Exec/ExecBase.hpp>
#include <Exec/BBase.hpp>
#include <Exec/x86/cpu_utils.hpp>
#include <atomic>

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
  std::atomic_flag locked = ATOMIC_FLAG_INIT;
  char *mName;

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
