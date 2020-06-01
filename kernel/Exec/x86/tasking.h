#ifndef TASKING_H
#define TASKING_H

#include <Exec/Types.h>

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

  volatile TUint64 upper_sp;
  volatile TUint64 lower_sp;
  volatile TUint64 tss;
  volatile TUint64 err_code;
  volatile TUint64 isr_num; //48

//  TInt errno;

  // segment/selector registers
  volatile TUint64 cs;
  volatile TUint64 ds;
  volatile TUint64 es;
  volatile TUint64 fs;
  volatile TUint64 gs;
  volatile TUint64 ss;

  volatile TUint8 fxsave[512+16];

  void Dump() {
    extern char *isr_names[];

#ifdef KERNEL
    dlog("task @ %x\n", this);

//    dlog(" isr_num: (%s) %x\n", isr_names[isr_num], isr_num);
    dlog("isr_num: (%s) %x\n", "ISR", isr_num);
    dlog("err_code: %x\n", err_code);
#if 0
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
#endif
    dlog("  stack:\n");
    TUint64 *stack = (TUint64 *)rsp;
    for (int i=0; i<10; i++) {
      dlog("    %016x: %016x\n", i, *stack++);
    }
    dlog("\n");
#endif
  }
} PACKED TTaskRegisters;

extern "C" TTaskRegisters *current_task;
extern "C" TTaskRegisters *next_task;

#endif
