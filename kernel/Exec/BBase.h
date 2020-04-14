#ifndef BBASE_H
#define BBASE_H

// Uncomment for production build
// #define PRODUCTION

#include <Exec/BTypes.h>
#include "Memory.h"
//#include <memory>

//extern void operator delete(TAny *ptr, unsigned long size);

TInt StringLength(const char *aString);
char *DuplicateString(const char *aString, EMemoryFlags aMemoryType = MEMF_PUBLIC);
void CopyString(char *aDestination, const char *aSource);
TInt CompareStrings(const char *aString1, const char *aString2);

void SetMemory8(TAny *aDestination, TUint8 aValue, TInt64 aCount);
void SetMemory16(TAny *aDestination, TUint16 aValue, TInt64 aCount);
void SetMemory32(TAny *aDestination, TUint32 aValue, TInt64 aCount);
void SetMemory64(TAny *aDestination, TUint64 aValue, TInt64 aCount);

/**
 * BBase is the base class for EVERYTHING.
 */
class BBase {
public:
  BBase();
  ~BBase();
public:
  void *operator new(size_t aSize);
  void *operator new[](size_t aSize);
  void operator delete(void *aMem);
  void operator delete[](void *aMem);
  // int __cxa_atexit(void (*func) (void *), void * arg, void * dso_handle) { return 0; }
};

extern TUint32 Milliseconds();


#endif
