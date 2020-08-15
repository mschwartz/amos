#ifndef AMOS_KERNEL_EXEC_MUTEX_H
#define AMOS_KERNEL_EXEC_MUTEX_H

/**
 * Spinlock mutex
 */

#include <Exec/BBase.hpp>

class Mutex : public BBase {
public:
  Mutex();
  ~Mutex();

public:
  TBool Try();
  void Acquire();
  void Release();

protected:
  volatile int mLock;
};

#endif
