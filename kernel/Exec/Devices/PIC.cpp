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
//  dprint("Constructing PIC\n");
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
//  ob(PIC1_DATA, 0xfb); [> mask everything, except 'slave' <]
//  ob(PIC2_DATA, 0xff); [> mask everything <]
  ob(PIC1_DATA, 0);
  ob(PIC2_DATA, 0);

  /* PIC initialized, all external interrupts disabled */
  // enable cascade, keyboard
  enable_interrupt(IRQ_SLAVE_PIC);
//  inb(0x21);
//  outb(PIC1_DATA, 0xf9);
//   enable RTC
//  inb(PIC2_DATA);
//  outb(PIC2_DATA, 0xfe);
}

PIC::~PIC() {
}

//void PIC::install_handler(uint16_t irq, INTERRUPT_HANDLER *h, void *data, const char *description) {
//  idt->install_handler(irq, h, data, description);
//}
void IRQ_set_mask(unsigned char IRQline) {
  uint16_t port;
  uint8_t value;

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
  uint16_t port;
  uint8_t value;

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

void PIC::disable_interrupt(uint16_t irq) {
  return;
  irq -= IRQ_OFFSET;
  dprint("enabled interrupt %d\n", irq);
  IRQ_set_mask(irq);
//  irq -= IRQ_OFFSET;
//  dprint("PIC1  %x\n", inb(PIC1_DATA));
//  dprint("PIC2  %x\n", inb(PIC2_DATA));
//  if (irq < 8) {
//    ob(PIC1_DATA, inb(PIC1_DATA) & ~(1 << irq));
//    dprint("Enabled interrupt %d %x\n", irq, inb(PIC1_DATA));
//  }
//  else {
//    ob(PIC2_DATA, inb(PIC2_DATA) & ~(1 << (irq - 8)));
//    dprint("Enabled interrupt %d %x\n", irq, inb(PIC2_DATA));
//  }
}

void PIC::enable_interrupt(uint16_t irq) {
  return;
  irq -= IRQ_OFFSET;
  dprint("enabled interrupt %d\n", irq);
  IRQ_clear_mask(irq);
//  if (irq < 8) {
//    ob(PIC1_DATA, inb(PIC1_DATA) | (1 << irq));
//  }
//  else {
//    ob(PIC2_DATA, inb(PIC2_DATA) | (1 << (irq - 8)));
//  }
}

void PIC::ack(uint16_t irq) {
  if (irq >= IRQ_OFFSET && irq < HW_INTERRUPTS) {
    ob(PIC1_CMD, PIC_EOI);
    if (irq >= IRQ_OFFSET + 8)
      ob(PIC2_CMD, PIC_EOI);
  }
}
