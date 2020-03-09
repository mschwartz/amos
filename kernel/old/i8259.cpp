/*! Intel 8259 programmable interrupt controller (PIC) */
#ifdef I8259

#include <cpu.h>
#include "i8259.h"

//#include <arch/interrupts.h>

#define PIC1_CMD 0x20  /* master PIC-a command port	*/
#define PIC2_CMD 0xA0  /* slave PIC-a command port	*/
#define PIC1_DATA 0x21 /* master PIC-a data port	*/
#define PIC2_DATA 0xA1 /* slave PIC-a data port	*/
#define PIC_EOI 0x20   /* EndOfInterrupt command	*/

#define ob(port, data) outb(data, port)
/*!
 * Initialize PIC
 */
void init_i8259() {
  ob(PIC1_CMD, 0x11); /* starting initialization  */
  ob(PIC2_CMD, 0x11);

  ob(PIC1_DATA, IRQ_OFFSET & 0xf8);       /* offset = +0x20 */
  ob(PIC2_DATA, (IRQ_OFFSET + 8) & 0xf8); /* offset = +0x28 */

  ob(PIC1_DATA, 0x4); /* continue initialization */
  ob(PIC2_DATA, 0x2);

  ob(PIC1_DATA, 0x1); /* 8086 mode */
  ob(PIC2_DATA, 0x1);

  ob(PIC1_DATA, 0xfb); /* mask everything, except 'slave' */
  ob(PIC2_DATA, 0xff); /* mask everything */

  /* PIC initialized, all external interrupts disabled */
}

/*!
 * Enable particular external interrupt in PIC
 * \param irq Interrupt request number
 */
void i8259_irq_enable(unsigned int irq) {
  irq -= IRQ_OFFSET;
  if (irq < 8)
    ob(PIC1_DATA, inb(PIC1_DATA) & ~(1 << irq));
  else
    ob(PIC2_DATA, inb(PIC2_DATA) & ~(1 << (irq - 8)));
}

/*!
 * Disable particular external interrupt in PIC
 * \param irq Interrupt request number
 */
void i8259_irq_disable(unsigned int irq) {
  irq -= IRQ_OFFSET;
  if (irq < 8)
    ob(PIC1_DATA, inb(PIC1_DATA) | (1 << irq));
  else
    ob(PIC2_DATA, inb(PIC2_DATA) | (1 << (irq - 8)));
}

/*!
 * At end of interrupt processing, re enable some ports in PIC
 * \param irq Interrupt request number
 */
void i8259_ack(unsigned int irq) {
  if (irq >= IRQ_OFFSET && irq < HW_INTERRUPTS) {
    ob(PIC1_CMD, PIC_EOI);
    if (irq >= IRQ_OFFSET + 8)
      ob(PIC2_CMD, PIC_EOI);
  }
}

/* Interrupts descriptions */
static char *arch_int_desc[] = {
  /* "exceptions" - interrupts generated in processor */
  "Divide Error",
  "Debug",
  "Non Maskable Interrupt",
  "Breakpoint",
  "Overflow",
  "Out of Bounds",
  "Invalid Opcode",
  "Device Not Available",
  "Double Fault",
  "Coprocessor Segment Overrun",
  "Invalid TSS",
  "Segment Not Present",
  "Stack Fault",
  "General Protection",
  "Page Fault",
  "Unknown Exception",
  "Coprocessor Fault",
  "Alignment Check",
  "Machine Check",
  "SIMD Fault",

  /* Intel reserved interrupts */
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",

  /* External interrupts (generated outside processor) */
  "IRQ_TIMER",
  "IRQ_KEYBOARD",
  "IRQ_SLAVE_PIC",
  "IRQ_COM2",
  "IRQ_COM1",
  "IRQ_LPT2",
  "IRQ_FLOPPY",
  "IRQ_LPT1",
  "IRQ_RT_CLOCK",
  "IRQ_MASTER_PIC",
  "IRQ_RESERVED1",
  "IRQ_RESERVED2",
  "IRQ_RESERVED3",
  "IRQ_COPROCESSOR",
  "IRQ_HARD_DISK",
  "IRQ_RESERVED4",

  /* Software interrupt, using first available = 48 */
  "Software interrupt"
};

/*!
 * Return info for requested interrupt number
 * \param n Interrupt number
 * \returns Pointer to description string
 */
static char *i8259_interrupt_description(unsigned int n) {
  if (n < INTERRUPTS)
    return arch_int_desc[n];
  else
    return "Unknown interrupt number";
}

#endif /* I8259 */
