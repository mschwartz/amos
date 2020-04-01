#ifndef KERNEL_PIC_H
#define KERNEL_PIC_H

/*!
 * 8259 Interrupt Controller
 */

#include <types.h>

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
  IRQ_RT_CLOCK,
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
//  void install_handler(uint16_t irq, INTERRUPT_HANDLER *h, void *data = nullptr, const char *description = "no description");
  void enable_interrupt(uint16_t irq);
  void disable_interrupt(uint16_t irq);
  void ack(uint16_t irq);
};

extern PIC *pic;

#endif
