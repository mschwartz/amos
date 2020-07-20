#include "interrupt.h"
#include <Exec/ExecBase.h>

TBool AtaInterrupt::Run(TAny *aData) {
  TUint64 num = (TUint64)aData;
#ifdef DEBUGME
  DLOG("  ----> Ata Interrupt %d\n", num);
#endif

  gExecBase.AckIRQ(num == 1 ? IRQ_ATA1 : IRQ_ATA2);
  // mTask->Signal(1 << mSignalBit);
  return ETrue;
}
