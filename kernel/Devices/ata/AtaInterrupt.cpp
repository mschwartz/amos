#include <Exec/ExecBase.h>
#include "AtaInterrupt.h"

#define DEBUGME
//#undef DEBUGME

AtaInterrupt::AtaInterrupt(AtaTask *aTask, TUint8 aSignalBit, TInt64 aDevice)
    : BInterrupt("ata.device", LIST_PRI_MAX, (TAny *)aDevice) {
  mTask = aTask;
  mSignalBit = aSignalBit;
  mDevice = aDevice;
}

TBool AtaInterrupt::Run(TAny *aData) {
  TUint64 num = (TUint64)aData;
#ifdef DEBUGME
  dlog("  ----> Ata Interrupt %d\n", num);
#endif

  gExecBase.AckIRQ(num == 1 ? IRQ_ATA1 : IRQ_ATA2);

  // signal task!
  mTask->Signal(1 << mSignalBit);
  return ETrue;
}
