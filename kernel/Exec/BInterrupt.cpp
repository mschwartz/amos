#include <Exec/BInterrupt.hpp>

BInterrupt::BInterrupt(const char *aNodeName, TInt64 aPri, TAny *aData) : BNodePri(aNodeName, aPri) {
  mData = aData;
}

BInterrupt::~BInterrupt() {
  //
}

