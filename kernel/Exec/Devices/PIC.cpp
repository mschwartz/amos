#include <x86/idt.h>
#include <x86/cpu.h>
#include <x86/kprint.h>
#include <x86/bochs.h>
#include <Devices/PIC.h>

// 8259 PIC Interrupt Controller

PIC *gPIC;

#define PIC1_CMD 0x20  /* master PIC-a command port	*/
#define PIC2_CMD 0xA0  /* slave PIC-a command port	*/
#define PIC1_DATA 0x21 /* master PIC-a data port	*/
#define PIC2_DATA 0xA1 /* slave PIC-a data port	*/
#define PIC_EOI 0x20   /* EndOfInterrupt command	*/

#define ob(port, data) outb(data, port)

PIC::PIC() {
  // remap PIC IRQs
  ob(PIC1_CMD, 0x11); /* starting initialization  */
  ob(PIC2_CMD, 0x11);

  ob(PIC1_DATA, 0x20); /* offset = +0x20 */
  ob(PIC2_DATA, 0x28);

  ob(PIC1_DATA, 0x4); /* continue initialization */
  ob(PIC2_DATA, 0x2);

  ob(PIC1_DATA, 0x1); /* 8086 mode */
  ob(PIC2_DATA, 0x1);

  // disable all interrupts
  ob(PIC1_DATA, 0);
  ob(PIC2_DATA, 0);

  // PIC initialized, all external interrupts disabled
  // enable cascade, keyboard
  enable_interrupt(IRQ_SLAVE_PIC);
}

PIC::~PIC() {
}

void IRQ_set_mask(unsigned char IRQline) {
  TUint16 port;
  TUint8 value;

  if (IRQline < 8) {
    port = PIC1_DATA;
  }
  else {
    port = PIC2_DATA;
    IRQline -= 8;
  }
  value = inb(port) | (1 << IRQline);
  outb(port, value);
}

void IRQ_clear_mask(unsigned char IRQline) {
  TUint16 port;
  TUint8 value;

  if (IRQline < 8) {
    port = PIC1_DATA;
  }
  else {
    port = PIC2_DATA;
    IRQline -= 8;
  }
  value = inb(port) & ~(1 << IRQline);
  outb(port, value);
}

void PIC::disable_interrupt(TUint16 irq) {
  irq -= IRQ_OFFSET;
  IRQ_set_mask(irq);
}

void PIC::enable_interrupt(TUint16 irq) {
  irq -= IRQ_OFFSET;
  IRQ_clear_mask(irq);
}

void PIC::ack(TUint16 irq) {
  if (irq >= IRQ_OFFSET && irq < HW_INTERRUPTS) {
    ob(PIC1_CMD, PIC_EOI);
    if (irq >= IRQ_OFFSET + 8)
      ob(PIC2_CMD, PIC_EOI);
  }
}
