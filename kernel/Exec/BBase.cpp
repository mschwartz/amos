#include <Exec/BBase.h>
#include <x86/bochs.h>

#ifdef KERNEL
#include <Exec/Memory.h>
#else
#include <stdlib.h>
#endif

BBase::BBase() {
  //
}

BBase::~BBase() {
  //
}

void *BBase::operator new(unsigned long aSize) {
#ifdef KERNEL
  return AllocMem(aSize, MEMF_PUBLIC);
#else
  return malloc(aSize);
#endif
}
void BBase::operator_delete(TAny *aMemory, unsigned long aSize) {
#ifdef KERNEL
  FreeMem(aMemory);
#else
  free(aMemory);
#endif
}
TUint32 Milliseconds() {
  return 0;
}

