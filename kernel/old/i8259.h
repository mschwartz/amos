/*! Intel 8259 programmable interrupt controller (PIC) */
#ifndef KERNEL_I8259
#define KERNEL_I8259

#pragma once

#define	IRQ_OFFSET	0x20	/* Interrupt offset for external interrupts */

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

#define SOFT_IRQ	HW_INTERRUPTS

#define NUM_IRQS	( SOFT_IRQ + 1 )

#define INTERRUPTS NUM_IRQS

extern void init_i8259();
extern void i8259_irq_enable ( unsigned int irq );
extern void i8259_irq_disable ( unsigned int irq );
extern void i8259_irq_disable ( unsigned int irq );
extern void i8259_ack ( unsigned int irq );
extern char *i8259_interrupt_description ( unsigned int n );

#endif /* I8259 */
