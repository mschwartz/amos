#ifndef BBASE_H
#define BBASE_H

// Uncomment for production build
// #define PRODUCTION

#include <Exec/Types.h>
#include <Exec/Memory.h>
//#include <memory>

//extern void operator delete(TAny *ptr, unsigned long size);

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
