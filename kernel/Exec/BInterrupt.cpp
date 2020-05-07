#include <Exec/BInterrupt.h>

BInterrupt::BInterrupt(const char *aNodeName, TInt64 aPri) : BNodePri(aNodeName, aPri) {
}

BInterrupt::~BInterrupt() {
  //
}

