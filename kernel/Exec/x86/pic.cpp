#define DEBUGME
// #undef DEBUGME

#include <Exec/x86/idt.hpp>
#include <Exec/x86/cpu_utils.hpp>
#include <Exec/x86/pic.hpp>

// 8259 PIC Interrupt Controller

#define PIC1_CMD 0x20  /* master PIC-a command port	*/
#define PIC2_CMD 0xA0  /* slave PIC-a command port	*/
#define PIC1_DATA 0x21 /* master PIC-a data port	*/
#define PIC2_DATA 0xA1 /* slave PIC-a data port	*/
#define PIC_EOI 0x20   /* EndOfInterrupt command	*/

inline static void wait_io() {
  outb(0x80, 0);
}

PIC::PIC() {
  // EnablePIC();
  DisablePIC();
}

PIC::~PIC() {
}

void PIC::EnablePIC() {
  // remap PIC IRQs
  outb(PIC1_CMD, 0x11); /* starting initialization  */
  wait_io();
  outb(PIC2_CMD, 0x11);
  wait_io();

  outb(PIC1_DATA, 32); /* offset = +0x20 */
  wait_io();
  outb(PIC2_DATA, 40);
  wait_io();

  outb(PIC1_DATA, 0x4); /* continue initialization */
  wait_io();
  outb(PIC2_DATA, 0x2);
  wait_io();

  outb(PIC1_DATA, 0x1); /* 8086 mode */
  outb(PIC2_DATA, 0x1);

  // disable/enable interrupts
  mMasterMask = 0x01;
  mSlaveMask = 0x00; // 0xff & ~0x20;

  outb(PIC1_DATA, mMasterMask);
  outb(PIC2_DATA, mSlaveMask);
  dlog("->>> PIC ENABLED\n");
}

void PIC::DisablePIC() {
  // ICW1
  outb(PIC1_CMD, 0x11);
  wait_io();
  outb(PIC2_CMD, 0x11);
  wait_io();

  // ICW2
  outb(PIC1_DATA, 0xe0);
  wait_io();
  outb(PIC2_DATA, 0xe8);
  wait_io();

  // ICW3
  outb(PIC1_DATA, 4);
  wait_io();
  outb(PIC2_DATA, 4);
  wait_io();

  // ICW4
  outb(PIC1_DATA, 1);
  wait_io();
  outb(PIC2_DATA, 1);
  wait_io();

  // ICW4
  outb(PIC1_DATA, 0xff);
  wait_io();
  outb(PIC2_DATA, 0xff);
  wait_io();

  mMasterMask = mSlaveMask = 0xff;
  dlog("->>> PIC DISABLED\n");
}

void PIC::EnableIRQ(TUint16 aIRQ) {
  bochs;
  return;
  aIRQ -= IRQ_OFFSET;
  if (aIRQ < 8) {
    mMasterMask &= ~(1 << aIRQ);
  }
  else {
    mSlaveMask &= ~(1 << aIRQ);
  }
  DLOG("    enable_interrupt %d masterMask(%x) slaveMask(%x)\n", aIRQ, mMasterMask, mSlaveMask);
  outb(PIC1_DATA, mMasterMask);
  outb(PIC2_DATA, mSlaveMask);
}

void PIC::DisableIRQ(TUint16 aIRQ) {
  bochs;
  return;
  aIRQ -= IRQ_OFFSET;
  if (aIRQ < 8) {
    mMasterMask |= (1 << aIRQ);
  }
  else {
    mSlaveMask |= (1 << aIRQ);
  }
  DLOG("    disable_interrupt %d master(%x) slave(%x)\n", aIRQ, mMasterMask, mSlaveMask);
  outb(PIC1_DATA, mMasterMask);
  outb(PIC2_DATA, mSlaveMask);
}

void PIC::AckIRQ(TUint16 aIRQ) {
  bochs;
  return;
  if (aIRQ >= IRQ_OFFSET && aIRQ < HW_INTERRUPTS) {
    if (aIRQ >= IRQ_OFFSET + 8)
      outb(PIC2_CMD, PIC_EOI);
  }
  outb(PIC1_CMD, PIC_EOI);
}
