#include <Exec/BBase.hpp>

#ifdef KERNEL
#include <Exec/Memory.hpp>
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

