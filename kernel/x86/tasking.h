#ifndef TASKING_H
#define TASKING_H

#include <Exec/BTypes.h>
#include <x86/bochs.h>

typedef struct Task {
  struct Task *next, *prev;
  // flags
  TUint64 rflags;
  // general purpose registers
  TUint64 rax;
  TUint64 rbx;
  TUint64 rcx;
  TUint64 rdx;
  TUint64 rdi;
  TUint64 rsi;
  // segment/selector registers
  TUint32 ds;
  TUint32 es;
  TUint32 fs;
  TUint32 gs;
  // instruction pointer
  TUint32 cs;
  TUint32 ss;
  TUint64 rip;
  // stack
  TUint64 rsp;
  TUint64 rbp;
  // flags

  TUint64 err_code;
  TUint64 isr_num; //48

  TInt errno;

  void Dump() {
    extern char *isr_names[];

    dprint("task @ %x\n", this);

//    dprint(" isr_num: (%s) %x\n", isr_names[isr_num], isr_num);
    dprint(" isr_num: (%s) %x\n", "ISR", isr_num);
    dprint(" err_code: %x\n", err_code);

    dprint(" rax: %x\n", rax);
    dprint(" rbx: %x\n", rbx);
    dprint(" rcx: %x\n", rcx);
    dprint(" rdx: %x\n", rdx);
    dprint(" rsi: %x\n", rsi);
    dprint(" rdi: %x\n", rdi);

    dprint(" rbp: %x\n", rbp);

    dprint(" cs: %x\n", cs);
    dprint(" ds: %x\n", ds);
    dprint(" es: %x\n", es);
    dprint(" fs: %x\n", fs);
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

class Scheduler {
public:
  Scheduler();
  ~Scheduler();
};

extern Scheduler *scheduler;
extern "C" task_t *current_task;

#endif
