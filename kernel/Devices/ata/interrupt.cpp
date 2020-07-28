#define DEBUGME
// #undef DEBUGME

#include "interrupt.hpp"
#include <Exec/ExecBase.hpp>
#include "task.hpp"

AtaInterrupt::AtaInterrupt(AtaTask *aTask, TUint8 aSignalBit, TInt64 aDevice)
      : BInterrupt("ata.device", LIST_PRI_MAX, (TAny *)aDevice) {
    mTask = aTask;
    mSignalBit = aSignalBit;
    mDevice = aDevice;
    DLOG("AtaInterrupt aTask(%s) aSignalBit(%d) aDevice(%d)\n", aTask->TaskName(), aSignalBit, aDevice);
  }

TBool AtaInterrupt::Run(TAny *aData) {
  TUint64 num = (TUint64)aData;
  DLOG("  ----> Ata Interrupt %d\n", num);

  gExecBase.AckIRQ(num == 1 ? IRQ_ATA1 : IRQ_ATA2);
  mTask->Signal(1 << mSignalBit);
  return ETrue;
}
