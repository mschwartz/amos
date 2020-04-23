#ifndef KERNEL_PIC_H
#define KERNEL_PIC_H

/*!
 * 8259 Interrupt Controller
 */

#include <Exec/Types.h>

#define IRQ_OFFSET 0x20 /* Interrupt offset for external interrupts */

/* External interrupts */
enum {
  IRQ_TIMER = IRQ_OFFSET,
  IRQ_KEYBOARD,
  IRQ_SLAVE_PIC,
  IRQ_COM2,
  IRQ_COM1,
  IRQ_LPT2,
  IRQ_FLOPPY,
  IRQ_LPT1,
  IRQ_RTC,
  IRQ_MASTER_PIC,
  IRQ_RESERVED1,
  IRQ_RESERVED2,
  IRQ_RESERVED3,
  IRQ_COPROCESSOR,
  IRQ_HARD_DISK,
  IRQ_RESERVED4,

  HW_INTERRUPTS
};

#define SOFT_IRQ HW_INTERRUPTS

#define NUM_IRQS (SOFT_IRQ + 1)

#define PIC_INTERRUPTS NUM_IRQS

typedef bool INTERRUPT_HANDLER(void *aData);

class PIC {
public:
  PIC();
  ~PIC();

public:
  void enable_interrupt(TUint16 irq);
  void disable_interrupt(TUint16 irq);
  void ack(TUint16 irq);
};

extern PIC *gPIC;

#endif
