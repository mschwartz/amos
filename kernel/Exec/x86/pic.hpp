#ifndef KERNEL_PIC_H
#define KERNEL_PIC_H

/*!
 * 8259 Interrupt Controller
 */

#include <Exec/ExecBase.hpp>

#define SOFT_IRQ HW_INTERRUPTS

#define NUM_IRQS (SOFT_IRQ + 1)

#define PIC_INTERRUPTS NUM_IRQS

typedef TBool TIRQHandler(void *aData);

class PIC {
public:
  PIC();
  ~PIC();

public:
  void EnableIRQ(TUint16 aIRQ);
  void DisableIRQ(TUint16 aIRQ);
  void AckIRQ(TUint16 aIRQ);
protected:
  TUint8 mMasterMask, mSlaveMask;
};

//extern PIC *gPIC;

#endif
