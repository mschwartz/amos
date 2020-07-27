#ifndef BBASE_H
#define BBASE_H

// Uncomment for production build
// #define PRODUCTION

#include <Types.hpp>
#include <Exec/Memory.hpp>

/**
 * BBase is the base class for EVERYTHING.
 */
class BBase {
public:
  BBase();
  ~BBase();
public:
#ifdef KERNEL
  void *operator new(unsigned long aSize);
  void *operator new[](unsigned long aSize);
  void operator delete(void *aMemory, unsigned long aSize);
  void operator delete[](void *aMemory, unsigned long aSize);
#endif
};

extern TUint32 Milliseconds();

#endif
