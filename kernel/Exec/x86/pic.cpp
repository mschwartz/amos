#include <Exec/x86/idt.h>
#include <Exec/x86/cpu.h>
#include <Exec/x86/pic.h>

// 8259 PIC Interrupt Controller

#define PIC1_CMD 0x20  /* master PIC-a command port	*/
#define PIC2_CMD 0xA0  /* slave PIC-a command port	*/
#define PIC1_DATA 0x21 /* master PIC-a data port	*/
#define PIC2_DATA 0xA1 /* slave PIC-a data port	*/
#define PIC_EOI 0x20   /* EndOfInterrupt command	*/

#define ob(port, data) outb(data, port)
inline static void wait_io() {
  outb(0, 0x80);
}

PIC::PIC() {
  // remap PIC IRQs
  ob(PIC1_CMD, 0x11); /* starting initialization  */
  wait_io();
  ob(PIC2_CMD, 0x11);
  wait_io();

  ob(PIC1_DATA, 32); /* offset = +0x20 */
  wait_io();
  ob(PIC2_DATA, 40);
  wait_io();

  ob(PIC1_DATA, 0x4); /* continue initialization */
  wait_io();
  ob(PIC2_DATA, 0x2);
  wait_io();

  ob(PIC1_DATA, 0x1); /* 8086 mode */
  ob(PIC2_DATA, 0x1);


#if 0
  // disable all interrupts
  mMasterMask = mSlaveMask = 0xff;

  ob(PIC1_DATA, 0xff);
  ob(PIC2_DATA, 0xff);

  // PIC initialized, all external interrupts disabled
  // enable cascade, keyboard
  EnableIRQ(IRQ_SLAVE_PIC);
#else
  //eenable all interrupts
  mMasterMask = mSlaveMask = 0x00;
  ob(PIC1_DATA, 0);
  ob(PIC2_DATA, 0);
#endif
  //  kprint("Enabled PIC\n");
  //  halt();
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
  outb(mMasterMask, PIC1_DATA);
  outb(mSlaveMask, PIC2_DATA);
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
  outb(mMasterMask, PIC1_DATA);
  outb(mSlaveMask, PIC2_DATA);
}

void PIC::AckIRQ(TUint16 aIRQ) {
  if (aIRQ >= IRQ_OFFSET && aIRQ < HW_INTERRUPTS) {
    if (aIRQ >= IRQ_OFFSET + 8)
      ob(PIC2_CMD, PIC_EOI);
  }
  ob(PIC1_CMD, PIC_EOI);
}
