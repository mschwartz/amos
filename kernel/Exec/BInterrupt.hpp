#ifndef EXEC_BINTERRUPT_H
#define EXEC_BINTERRUPT_H

#include <Types/BList.hpp>
#include <Exec/SpinLock.hpp>

/* External interrupts */
#define IRQ_OFFSET 0x20 /* Interrupt offset for external interrupts */
enum {
  IRQ_APIC_TIMER = IRQ_OFFSET,    // 0
  IRQ_KEYBOARD,                   // 1
  IRQ_TIMER,                      // 2
  IRQ_SLAVE_PIC = IRQ_OFFSET + 2, // 2
  IRQ_COM2,                       // 3
  IRQ_COM1,                       // 4
  IRQ_LPT2,                       // 5
  IRQ_FLOPPY,                     // 6
  IRQ_LPT1,                       // 7
  IRQ_RTC,                        // 8
  IRQ_MASTER_PIC,                 // 9
  IRQ_RESERVED1,                  // 10
  IRQ_RESERVED2,                  // 11
  IRQ_MOUSE,                      // 12
  IRQ_COPROCESSOR,                // 13
  IRQ_ATA1,                       // 14
  IRQ_ATA2,                       // 15

  HW_INTERRUPTS
};

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
  EApicTimerIRQ = 0x20,
  EKeyboardIRQ = 0x21,
  ETimerIRQ = 0x22,
  ESlavePicIRQ = 0x22,
  ECom2IRQ,
  ECom1IRQ,
  ELpt2IRQ,
  EFloppyIRQ,
  ELpt1IRQ,
  ERtcClockIRQ,
  EMasterPicIRQ,
  EReserved1IRQ,
  EReserved2IRQ,
  EMouseIRQ,
  ECoprocessorIRQ,
  EAta1IRQ,
  EAta2IRQ,
  //
  ETrap0,
  EMaxInterrupts,

};

/*
 * To install an interrupt handler, inherit from BInterrupt, create an instance of your class, and call
 * gExecBase.SetIntVector() with it.
 */
class ExecBase;

class BInterrupt : public BNodePri {
  friend ExecBase;

public:
  BInterrupt(const char *aNodeName, TInt64 aPri, TAny *aData = ENull);
  ~BInterrupt();

public:
  /**
      * You must imeplement a Run() method in your inherited class.  It returns ETrue if it handled the interrupt
      * and no more interrupt handlers in the chain are to be executed.
      */
  virtual TBool Run(TAny *aData) = 0;

protected:
  TAny *mData;
};

class BInterruptList : public BListPri {
public:
  BInterruptList() : BListPri("Interrupt List") {
  }
  ~BInterruptList() {}

public:
  void Lock() { mSpinLock.Acquire(); }
  void Unlock() { mSpinLock.Release(); }

protected:
  SpinLock mSpinLock;
};

#endif
