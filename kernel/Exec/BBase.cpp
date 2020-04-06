#include <Exec/BBase.h>
#include <x86/bochs.h>

void operator delete(TAny *ptr, unsigned long size) { }

#if 0
BBase::BBase() {
  //
}

BBase::~BBase() {
  //
}
#endif

TUint32 Milliseconds() {
  return 0;
}

