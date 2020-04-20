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

    dprint("task @ %x\n", this);

//    dprint(" isr_num: (%s) %x\n", isr_names[isr_num], isr_num);
    dprint(" isr_num: (%s) %x\n", "ISR", isr_num);
    dprint(" err_code: %x\n", err_code);

    dprint(" rax: %x ", rax);
    dprint(" rbx: %x ", rbx);
    dprint(" rcx: %x ", rcx);
    dprint(" rdx: %x ", rdx);
    dprint(" rsi: %x ", rsi);
    dprint(" rdi: %x ", rdi);
    dprint(" rbp: %x\n", rbp);

    dprint(" cs: %x ", cs);
    dprint(" ds: %x ", ds);
    dprint(" es: %x ", es);
    dprint(" fs: %x ", fs);
    dprint(" gs: %x\n", gs);

    dprint(" rip: %x\n", rip);

    dprint("  stack:\n");
    TUint64 *stack = (TUint64 *)rsp;
    for (int i=0; i<10; i++) {
      dprint("    %x: %x\n", i, *stack++);
    }
    dprint("\n");
  }
} PACKED task_t;

extern "C" task_t *current_task;
extern "C" task_t *next_task;

#endif
