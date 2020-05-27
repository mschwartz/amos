#include <Exec/x86/idt.h>
#include <Exec/x86/cpu.h>
#include <Exec/x86/pic.h>

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

#if 0
  //eenable all interrupts
  mMasterMask = mSlaveMask = 0x00;
#else
  // disable all interrupts
  mMasterMask = mSlaveMask = 0xff;
#endif
  outb(PIC1_DATA, mMasterMask);
  outb(PIC2_DATA, mSlaveMask);
  sti();
}

PIC::~PIC() {
}

void PIC::EnableIRQ(TUint16 aIRQ) {
  //  dlog("    enable_interrupt %d\n", aIRQ);
  aIRQ -= IRQ_OFFSET;
  if (aIRQ < 8) {
    mMasterMask &= ~(1 << aIRQ);
  }
  else {
    mSlaveMask &= ~(1 << aIRQ);
  }
  outb(PIC1_DATA, mMasterMask);
  outb(PIC2_DATA, mSlaveMask);
}

void PIC::DisableIRQ(TUint16 aIRQ) {
  //  dlog("    disable_interrupt %d\n", aIRQ);
  aIRQ -= IRQ_OFFSET;
  if (aIRQ < 8) {
    mMasterMask |= (1 << aIRQ);
  }
  else {
    mSlaveMask |= (1 << aIRQ);
  }
  outb(PIC1_DATA, mMasterMask);
  outb(PIC2_DATA, mSlaveMask);
}

void PIC::AckIRQ(TUint16 aIRQ) {
  if (aIRQ >= IRQ_OFFSET && aIRQ < HW_INTERRUPTS) {
    if (aIRQ >= IRQ_OFFSET + 8)
      outb(PIC2_CMD, PIC_EOI);
  }
  outb(PIC1_CMD, PIC_EOI);
}
