#include <Exec/BBase.h>
#include <x86/bochs.h>

#ifdef KERNEL
#include <Exec/Memory.h>
#endif

BBase::BBase() {
  //
}

BBase::~BBase() {
  //
}

#ifdef KERNEL
extern "C" void __cxa_pure_virtual() {}
#endif


#ifdef KERNEL
void *BBase::operator new(unsigned long aSize) {
  return AllocMem(aSize, MEMF_PUBLIC);
}
#endif

#ifdef KERNEL
void *BBase::operator new[](unsigned long aSize) {
  return AllocMem(aSize, MEMF_PUBLIC);
}
#endif

#ifdef KERNEL
void BBase::operator delete(TAny *aMemory, unsigned long aSize) {
  FreeMem(aMemory);
}
#endif

#ifdef KERNEL
void BBase::operator delete[](TAny *aMemory, unsigned long aSize) {
  FreeMem(aMemory);
}
#endif

TUint32 Milliseconds() {
  return 0;
}

