#ifndef EXEC_BINTERRUPT_H
#define EXEC_BINTERRUPT_H

#include <Exec/BList.h>

enum EInterruptNumber {
  // Exceptions
  EDivideError,
  EDebug,
  ENmi,
  EInt3,
  EOverflow,
  EOutOfBounds,
  EInvalidOpCode,
  EDeviceNotAvailable,
  EDoubleFault,
  ECoprocessorSegmentOverrun,
  EInvalidTSS,
  ESegmentNotPresent,
  EStackFault,
  EGeneralProtection,
  EPageFault,
  EUnknown,
  ECoprocessorFault,
  EAlignmentCheck,
  EMachineCheck,
  ESimdFault,
  EVirtualization,
  ESecurity,
  // IRQ
  ETimerIRQ = 0x20,
  EKeyboardIRQ = 0x21,
  ESlavePicIRQ,
  ECom2IRQ,
  ECom1IRQ,
  ELpt2IRQ,
  EFloppyIRQ,
  ELpt1IRQ,
  ERtClockIRQ,
  EMasterPicIRQ,
  EReserved1IRQ,
  EReserved2IRQ,
  EReserved3IRQ,
  ECoprocessorIRQ,
  EHardDiskIRQ,
  EReserved4IRQ,
  //
  ETrap0,
  EMaxInterrupts,

};

/*
 * To install an interrupt handler, inherit from BInterrupt, create an instance of your class, and call
 * gExecBase.SetIntVector() with it.
 */
class BInterrupt : public BNodePri {
public:
  BInterrupt(const char *aNodeName, TInt64 aPri);
  ~BInterrupt();

public:
  /**
      * You must imeplement a Run() method in your inherited class.  It returns ETrue if it handled the interrupt
      * and no more interrupt handlers in the chain are to be executed.
      */
  virtual TBool Run(TAny *aData) = 0;
};

class BInterruptList : public BListPri {
public:
  BInterruptList() : BListPri("Interrupt List") {}
  ~BInterruptList() {}
};

#endif
