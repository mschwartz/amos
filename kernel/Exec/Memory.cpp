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

void *operator new(size_t size) { return AllocMem(size, MEMF_SLOW); }

void *operator new[](size_t size) { return AllocMem(size, MEMF_SLOW); }

void operator delete(void *ptr) {
  FreeMem(ptr);
}

void operator delete[](void *ptr) {
  FreeMem(ptr);
}
