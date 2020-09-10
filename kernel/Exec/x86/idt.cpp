#include <Exec/ExecBase.hpp>
#include <Exec/x86/idt.hpp>
#include <posix/string.h>
#include <Exec/x86/tasking.hpp>
#include <Exec/x86/kernel_memory.hpp>

extern "C" void set_vector(void *idt_vector, void (*offset)(), TUint16 selector, uint8_t flags);
extern "C" void load_idtr(void *ptr);

// exceptions
extern "C" TUint64 isr0;
extern "C" TUint64 isr1;
extern "C" TUint64 isr2;
extern "C" TUint64 isr3;
extern "C" TUint64 isr4;
extern "C" TUint64 isr5;
extern "C" TUint64 isr6;
extern "C" TUint64 isr7;
extern "C" TUint64 isr8;
extern "C" TUint64 isr9;
extern "C" TUint64 isr10;
extern "C" TUint64 isr11;
extern "C" TUint64 isr12;
extern "C" TUint64 isr13;
extern "C" TUint64 isr14;
extern "C" TUint64 isr15;
extern "C" TUint64 isr16;
extern "C" TUint64 isr17;
extern "C" TUint64 isr18;
extern "C" TUint64 isr19;
extern "C" TUint64 isr20;
extern "C" TUint64 isr21;
extern "C" TUint64 isr22;
extern "C" TUint64 isr23;
extern "C" TUint64 isr24;
extern "C" TUint64 isr25;
extern "C" TUint64 isr26;
extern "C" TUint64 isr27;
extern "C" TUint64 isr28;
extern "C" TUint64 isr29;
extern "C" TUint64 isr30;
extern "C" TUint64 isr31;

// IRQs
extern "C" TUint64 isr32; // the timer irq (0)
extern "C" TUint64 isr33; // the kb irq (1)
extern "C" TUint64 isr34;
extern "C" TUint64 isr35;
extern "C" TUint64 isr36;
extern "C" TUint64 isr37;
extern "C" TUint64 isr38;
extern "C" TUint64 isr39;
extern "C" TUint64 isr40;
extern "C" TUint64 isr41;
extern "C" TUint64 isr42;
extern "C" TUint64 isr43;
extern "C" TUint64 isr44;
extern "C" TUint64 isr45;
extern "C" TUint64 isr46; // ata 0,0 (14)
extern "C" TUint64 isr47; // ata 0,1 (15)

extern "C" TUint64 isr48; // unexpected
extern "C" TUint64 isr49; // unexpected
extern "C" TUint64 isr50; // unexpected
extern "C" TUint64 isr51; // unexpected
extern "C" TUint64 isr52; // unexpected
extern "C" TUint64 isr53; // unexpected
extern "C" TUint64 isr54; // unexpected
extern "C" TUint64 isr55; // unexpected
extern "C" TUint64 isr56; // unexpected
extern "C" TUint64 isr57; // unexpected
extern "C" TUint64 isr58; // unexpected
extern "C" TUint64 isr59; // unexpected
extern "C" TUint64 isr60; // unexpected

extern "C" TUint64 isr128; // syscall (0x80, or 0x60 + 32)
extern "C" TUint64 isr130; // interrupt scheduler

static TIsrHandler interrupt_handlers[INTERRUPTS];
static TBool interrupt_enabled[256];

/**
 * kernel_isr
 *
 * This method is called from the assembly ISR handler(s) and Exception handlers.
 */
extern "C" TBool kernel_isr(TInt64 aIsrNumber) {
  cli();
  dprint("\n\nkernel_isr %d(%s)\n\n", aIsrNumber, IDT::InterruptDescription(aIsrNumber));

  if (!interrupt_enabled[aIsrNumber] && aIsrNumber != 48) {
    gExecBase.AckIRQ(aIsrNumber);
    return ETrue;
  }

  CPU *cpu = GetCPU();
  // if (cpu && cpu->mApicId) {
  //   dlog("kernel_isr %d\n", aIsrNumber);
  //   // bochs;
  // }

  TTaskContext *current_task = GetCurrentTask();
  TIsrHandler *info = &interrupt_handlers[current_task->isr_num];
  if (!info->mHandler) {
    const char *desc = IDT::InterruptDescription(current_task->isr_num);
    dlog("no handler: %d(%x) %s\n", aIsrNumber, current_task->isr_num, desc);
    gExecBase.AckIRQ(aIsrNumber);
    return false;
  }

  bool ret = info->mHandler(info->mInterruptNumber, info->mData);
  gExecBase.AckIRQ(aIsrNumber);
  return ret;
};

void IDT::EnableInterrupt(TUint16 aInterruptNumber) {
  dlog("IDT enable interrupt(%d)\n", aInterruptNumber);
  interrupt_enabled[aInterruptNumber] = ETrue;
}
void IDT::DisableInterrupt(TUint16 aInterruptNumber) {
  dlog("IDT disable interrupt(%d)\n", aInterruptNumber);
  interrupt_enabled[aInterruptNumber] = EFalse;
}

#pragma pack(1)
typedef struct {
  TUint16 mAddressLow;
  TUint16 mSelector;
  TUint8 mIst : 3;
  TUint8 mZeroes1 : 5;
  TUint8 mType : 1;
  TUint8 mOnes1 : 3;
  TUint8 mZeroes2 : 1;
  TUint8 mDpl : 2;
  TUint8 mPresent : 1;
  TUint16 mAddressMiddle;
  TUint32 mAddressHigh;
  TUint32 mZeroes3;
} PACKED TIdtEntry;

typedef struct {
  TUint16 limit;
  TIdtEntry *base;
} PACKED TIdtPtr;
#pragma pack(0)

const TInt IDT_SIZE = 256;

static TIdtEntry idt_entries[IDT_SIZE] ALIGN16;
static TIdtPtr idt_ptr ALIGN16;

static void set_entry(TInt aIndex, TUint64 aVec, TInt aIst = 2) {
  TIdtEntry &e = idt_entries[aIndex];
  e.mAddressLow = (TUint16)aVec;
  e.mAddressMiddle = (TUint16)(aVec >> 16);
  e.mAddressHigh = (TUint32)(aVec >> 32);
  e.mZeroes1 = 0;
  e.mZeroes2 = 0;
  e.mType = 0;
  e.mIst = aIst;
  e.mDpl = 0;
  e.mOnes1 = 0b111;
  e.mSelector = 8; // SEG_KCODE
  e.mPresent = 1;
}

IDT::IDT() {
  DISABLE;
  // initialize C ISRs
  for (TInt i = 0; i < INTERRUPTS; i++) {
    interrupt_handlers[i].Set(i, nullptr, nullptr, "Not installed");
  }

  for (TInt i = 0; i < 256; i++) {
    mEnabled[i] = EFalse;
  }

  // EXCEPTIONS
  set_entry(0, isr0);
  set_entry(1, isr1);
  set_entry(2, isr2);
  set_entry(3, isr3);
  set_entry(4, isr4);
  set_entry(5, isr5);
  set_entry(6, isr6);
  set_entry(7, isr7);
  set_entry(8, isr8);
  set_entry(9, isr9);
  set_entry(10, isr10);
  set_entry(11, isr11);
  set_entry(12, isr12);
  set_entry(13, isr13);
  set_entry(14, isr14);
  set_entry(15, isr15);
  set_entry(16, isr16);
  set_entry(17, isr17);
  set_entry(18, isr18);
  set_entry(19, isr19);
  set_entry(20, isr20);
  set_entry(21, isr21);
  set_entry(22, isr22);
  set_entry(23, isr23);
  set_entry(24, isr24);
  set_entry(25, isr25);
  set_entry(26, isr26);
  set_entry(27, isr27);
  set_entry(28, isr28);
  set_entry(29, isr29);
  set_entry(30, isr30);
  set_entry(31, isr31);

  // IRQs
  set_entry(32, isr32, 0); // 00 timer
  set_entry(33, isr33, 1); // 01 keyboard
  set_entry(34, isr34, 1); // 02 slave
  set_entry(35, isr35, 1); // 03 com2
  set_entry(36, isr36, 1); // 04 com1
  set_entry(37, isr37, 1); // 05 lpt2
  set_entry(38, isr38, 1); // 06 floppy
  set_entry(39, isr39, 1); // 07 lpt1
  set_entry(40, isr40, 1); // 08 RTC
  set_entry(41, isr41, 1); // 09 MASTER_PIC
  set_entry(42, isr42, 1); // 10 RESERVED
  set_entry(43, isr43, 1); // 11 RESERVED
  set_entry(44, isr44, 1); // 12 MOUSE
  set_entry(45, isr45, 1); // 13 COPROCESSOR
  set_entry(46, isr46, 1); // 14 HARD DISK
  set_entry(47, isr47, 1); // 15 RESERVED

  set_entry(48, isr48, 3); // Schedule TRAP

  for (int i = 49; i < IDT_SIZE; i++) {
    set_entry(i, isr49, 2);
  }
  set_entry(128, isr128);
  set_entry(130, isr130);

  mAlive = ETrue;
  ENABLE;
}

void IDT::Install() {
  idt_ptr.limit = sizeof(TIdtEntry) * IDT_SIZE - 1;
  idt_ptr.base = &idt_entries[0];
  load_idtr(&idt_ptr);
}

IDT::~IDT() {
  DisableInterrupts();
}

void IDT::InstallHandler(uint8_t index, TInterruptHandler *handler, void *aData, const char *description) {
  interrupt_handlers[index].Set(index, handler, aData, description);
}

static const char *int_desc[] = {
  /* "exceptions" - interrupts generated in processor */
  /* 00 */ "Divide Error",
  /* 01 */ "Debug",
  /* 02 */ "Non Maskable Interrupt",
  /* 03 */ "Breakpoint",
  /* 04 */ "Overflow",
  /* 05 */ "Out of Bounds",
  /* 06 */ "Invalid Opcode",
  /* 07 */ "Device Not Available",
  /* 08 */ "Double Fault",
  /* 09 */ "Coprocessor Segment Overrun",
  /* 10 */ "Invalid TSS",
  /* 11 */ "Segment Not Present",
  /* 12 */ "Stack Fault",
  /* 13 */ "General Protection",
  /* 14 */ "Page Fault",
  /* 15 */ "Unknown Exception",
  /* 16 */ "Coprocessor Fault",
  /* 17 */ "Alignment Check",
  /* 18 */ "Machine Check",
  /* 19 */ "SIMD Fault",

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
  "IRQ_APIC_TIMER",
  "IRQ_KEYBOARD",
  // "IRQ_SLAVE_PIC",
  "IRQ_TIMER",
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

const char *IDT::InterruptDescription(TUint16 n) {
  /* Interrupts descriptions */

  if (n < INTERRUPTS)
    return int_desc[n];
  else
    return "Unknown interrupt number";
}
