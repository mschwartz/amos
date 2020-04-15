#ifndef EXEC_MEMORY_H
#define EXEC_MEMORY_H

#include <Exec/BTypes.h>

// these flags may be ORed together
enum EMemoryFlags {
  MEMF_ANY,
  MEMF_PUBLIC,
  MEMF_CHIP,
  MEMF_FAST,
  MEMF_CLEAR
};

extern TAny *AllocMem(long aSize, int aFlags = MEMF_ANY);
extern void FreeMem(TAny *aMemory);

extern void *operator new(unsigned long aSize);
extern void operator delete(void *aMemory, unsigned long aSize);

#endif
