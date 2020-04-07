#ifndef BBASE_H
#define BBASE_H

// Uncomment for production build
// #define PRODUCTION

#include <Exec/BTypes.h>
#include "Memory.h"
//#include <memory>

extern void operator delete(TAny *ptr, unsigned long size);

/**
 * BBase is the base class for EVERYTHING.
 */
class BBase {
public:
  BBase() {}
  virtual ~BBase() {}

public:
  // int __cxa_atexit(void (*func) (void *), void * arg, void * dso_handle) { return 0; }

//  void *operator new(size_t size);
//  void *operator new[](size_t size);
//  void operator delete(void *ptr);
//  void operator delete[](void *ptr);
};

extern TUint32 Milliseconds();

#endif
