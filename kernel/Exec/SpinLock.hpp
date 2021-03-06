#ifndef AMOS_KERNEL_EXEC_MUTEX_H
#define AMOS_KERNEL_EXEC_MUTEX_H

/**
 * Spinlock mutex
 */

#include <Exec/BBase.hpp>

typedef TUint32 TSpinLock;

class BTask;

class SpinLock : public BBase {
public:
  SpinLock();
  ~SpinLock();

public:
  TBool Try();
  void Acquire();
  void Release();

protected:
  volatile TSpinLock mLock;
  TUint64 mFlags;
};

#endif
