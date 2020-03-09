/*!
 * Interrupts
 *
 * kernel_isr.asm sets up the IDT with 256 interrupt handlers and loads it into the CPU.
 *
 * The interrupt handlers save the registers, and call a C/C++ master handler with the interrupt/IRQ number as its argument.
 *
 * In C/C++ space, an array of pointers to functions is maintained.  The index is the IRQ number, the function is the
 * interrupt handler.
 *
 * The master function knows nothing about the 8259.  It is up to the C/C++ handler to ACK the 8259 interrupt before
 * returning.
 *
 * The kernel_isr handlers return from interrupt context properly.
 */
#ifndef KERNEL_INTERRUPTS_H
#define KERNEL_INTERRUPTS_H

#include "types.h"

/*******************************************************/

typedef struct registers {
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;

	uint32_t edi; // 16
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx; // 36
	uint32_t ecx;
	uint32_t eax;

	uint32_t isr_num; //48
	uint32_t err_code;

	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t useresp;
	uint32_t ss;
} __attribute__((packed)) registers_t;

typedef bool KERNEL_INTERRUPT_HANDLER(registers_t *registers);

extern void init_interrupts();
extern void install_interrupt_handler(int index, KERNEL_INTERRUPT_HANDLER *handler);

#endif
