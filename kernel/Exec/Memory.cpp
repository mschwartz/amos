#include <Exec/Memory.h>
#include <posix/string.h>
#include <posix/malloc.h>

void *AllocMem(TUint64 aSize, TInt aFlags) {
  TUint8 *mem = (TUint8 *)malloc(aSize);
  if (aFlags & MEMF_CLEAR) {
    memset(mem, 0, aSize);
  }
  return (TAny *)mem;

}

void FreeMem(TAny *memory) {
  free(memory);
}

void *operator new(unsigned long aSize) {
#ifdef KERNEL
  return AllocMem(aSize, MEMF_PUBLIC);
#else
  return malloc(aSize);
#endif
}

void operator delete(void *aMemory, unsigned long aSize) {
#ifdef KERNEL
  FreeMem(aMemory);
#else
  free(aMemory);
#endif
}

