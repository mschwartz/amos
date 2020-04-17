#include <Exec/BInterrupt.h>
#include <x86/bochs.h>

BInterrupt::BInterrupt(const char *aNodeName, TInt64 aPri) : BNodePri(aNodeName, aPri) {
}

BInterrupt::~BInterrupt() {
  //
}

