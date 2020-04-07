#ifndef EXEC_MEMORY_H
#define EXEC_MEMORY_H

#include <Exec/BTypes.h>
#include <stddef.h>
// these flags may be ORed together
enum EMemoryFlags {
  MEMF_ANY,
  MEMF_PUBLIC,
  MEMF_CHIP,
  MEMF_FAST,
  MEMF_SLOW,
  MEMF_CLEAR
};

extern TAny *AllocMem(unsigned long aSize, int aFlags = MEMF_ANY);
extern void FreeMem(TAny *aMemory);

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *ptr);
void operator delete[](void *ptr);

#if 0
void *operator new(size_t size) { return AllocMem(size, MEMF_SLOW); }

void *operator new[](size_t size) { return AllocMem(size, MEMF_SLOW); }

void operator delete(void *ptr) {
  FreeMem(ptr);
}

void operator delete[](void *ptr) {
  FreeMem(ptr);
}
#endif

#endif
