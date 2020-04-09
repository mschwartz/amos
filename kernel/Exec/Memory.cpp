#include <Exec/Memory.h>
#ifdef TOOL
#include <string.h>
#include <stdlib.h>
#else
#include <posix/string.h>
#include <posix/malloc.h>
#endif

//#ifdef TOOL
//extern void memset(void *dst, uint8_t v, size_t size);
//#endif

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

void *operator new(size_t size) { return AllocMem(size, MEMF_SLOW); }

void *operator new[](size_t size) { return AllocMem(size, MEMF_SLOW); }

void operator delete(void *ptr) {
  FreeMem(ptr);
}

void operator delete[](void *ptr) {
  FreeMem(ptr);
}
