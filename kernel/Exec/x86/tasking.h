#ifndef TASKING_H
#define TASKING_H

#include <Exec/BTypes.h>
#include <x86/bochs.h>

typedef struct Task {
  // flags
  volatile TUint64 rflags;
  // general purpose registers
  volatile TUint64 rax;
  volatile TUint64 rbx;
  volatile TUint64 rcx;
  volatile TUint64 rdx;
  volatile TUint64 rsi;
  volatile TUint64 rdi;

  volatile TUint64 r8;
  volatile TUint64 r9;
  volatile TUint64 r10;
  volatile TUint64 r11;
  volatile TUint64 r12;
  volatile TUint64 r13;
  volatile TUint64 r14;
  volatile TUint64 r15;

  // instruction pointer
  volatile TUint64 rip;
  // stack
  volatile TUint64 rsp;
  volatile TUint64 rbp;
  // flags

  volatile TUint64 err_code;
  volatile TUint64 isr_num; //48

  TInt errno;

  // segment/selector registers
  volatile TUint16 cs;
  volatile TUint16 ds;
  volatile TUint16 es;
  volatile TUint16 fs;
  volatile TUint16 gs;
  volatile TUint16 ss;

  void Dump() {
    extern char *isr_names[];

    dlog("task @ %x\n", this);

//    dlog(" isr_num: (%s) %x\n", isr_names[isr_num], isr_num);
    dlog("isr_num: (%s) %x\n", "ISR", isr_num);
    dlog("err_code: %x\n", err_code);

    dlog("    rax: %016x ", rax);
    dlog("    rbx: %016x ", rbx);
    dlog("    rcx: %016x ", rcx);
    dlog("    rdx: %016x ", rdx);
    dlog("    rsi: %016x ", rsi);
    dlog("    rdi: %016x ", rdi);
    dlog("    rbp: %016x\n", rbp);

    dlog("     cs: %016x ", cs);
    dlog("     ds: %016x ", ds);
    dlog("     es: %016x ", es);
    dlog("     fs: %016x ", fs);
    dlog("     gs: %016x\n", gs);

    dlog("    rip: %016x\n", rip);

    dlog("  stack:\n");
    TUint64 *stack = (TUint64 *)rsp;
    for (int i=0; i<10; i++) {
      dlog("    %016x: %016x\n", i, *stack++);
    }
    dlog("\n");
  }
} PACKED TTaskRegisters;

extern "C" TTaskRegisters *current_task;
extern "C" TTaskRegisters *next_task;

#endif
