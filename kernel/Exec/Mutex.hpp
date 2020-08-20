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
  void Acquire(const char *aMessage = ENull);
  void Release(const char *amessage = ENull);

protected:
  TUint64 mLock;
};

#endif
