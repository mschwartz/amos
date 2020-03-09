#include <kprint.h>
//#include <kernel_memory.h>
#include <kernel_interrupts.h>
#include <cpu.h>
#include <i8259.h>
#include <bochs.h>

extern void i8259_init();

KERNEL_INTERRUPT_HANDLER *interrupt_handlers[INTERRUPTS];
//bool (*interrupt_handlers[INTERRUPTS]());

void install_interrupt_handler(int index, KERNEL_INTERRUPT_HANDLER *handler){
  interrupt_handlers[index] = handler;
};

//extern "C" void populate_idt();
extern "C" bool kernel_isr(uint64_t isr) {
  dprint("ISR %d\n", isr);
  bochs
  halt();
  return true;
}

void init_interrupts() {
  disable_interrupts();
//  populate_idt();
  kprint("populated idt\n");
  i8259_init();
  kprint("init 8259\n");
  while (1)
    ;
}

#if 0
#define PIC1_CMD 0x20  /* master PIC-a command port	*/
#define PIC2_CMD 0xA0  /* slave PIC-a command port	*/
#define PIC1_DATA 0x21 /* master PIC-a data port	*/
#define PIC2_DATA 0xA1 /* slave PIC-a data port	*/
#define PIC_EOI 0x20   /* EndOfInterrupt command	*/

/*!
 * Initialize PIC
 */
void i8259_init() {
  outb(PIC1_CMD, 0x11); /* starting initialization  */
  outb(PIC2_CMD, 0x11);

  outb(PIC1_DATA, IRQ_OFFSET & 0xf8);       /* offset = +0x20 */
  outb(PIC2_DATA, (IRQ_OFFSET + 8) & 0xf8); /* offset = +0x28 */

  outb(PIC1_DATA, 0x4); /* continue initialization */
  outb(PIC2_DATA, 0x2);

  outb(PIC1_DATA, 0x1); /* 8086 mode */
  outb(PIC2_DATA, 0x1);

  outb(PIC1_DATA, 0xfb); /* mask everything, except 'slave' */
  outb(PIC2_DATA, 0xff); /* mask everything */

  /* PIC initialized, all external interrupts disabled */
}

/*!
 * Enable particular external interrupt in PIC
 * \param irq Interrupt request number
 */
 void i8259_irq_enable(unsigned int irq) {
  irq -= IRQ_OFFSET;
  if (irq < 8)
    outb(PIC1_DATA, inb(PIC1_DATA) & ~(1 << irq));
  else
    outb(PIC2_DATA, inb(PIC2_DATA) & ~(1 << (irq - 8)));
}

/*!
 * Disable particular external interrupt in PIC
 * \param irq Interrupt request number
 */
 void i8259_irq_disable(unsigned int irq) {
  irq -= IRQ_OFFSET;
  if (irq < 8)
    outb(PIC1_DATA, inb(PIC1_DATA) | (1 << irq));
  else
    outb(PIC2_DATA, inb(PIC2_DATA) | (1 << (irq - 8)));
}

/*!
 * At end of interrupt processing, re enable some ports in PIC
 * \param irq Interrupt request number
 */
 void i8259_at_exit(unsigned int irq) {
  if (irq >= IRQ_OFFSET && irq < HW_INTERRUPTS) {
    outb(PIC1_CMD, PIC_EOI);
    if (irq >= IRQ_OFFSET + 8)
      outb(PIC2_CMD, PIC_EOI);
  }
}

/* Interrupts descriptions */
static const char *int_desc[] = {
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
static const char *i8259_interrupt_description(unsigned int n) {
  if (n < INTERRUPTS)
    return int_desc[n];
  else
    return "Unknown interrupt number";
}
#endif
