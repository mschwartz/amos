#ifndef TASKING_H
#define TASKING_H

#include <types.h>
#include <bochs.h>

typedef struct Task {
  struct Task *next, *prev;
  // flags
  uint64_t rflags;
  // general purpose registers
  uint64_t rax;
  uint64_t rbx;
  uint64_t rcx;
  uint64_t rdx;
  uint64_t rdi;
  uint64_t rsi;
  // segment/selector registers
  uint32_t ds;
  uint32_t es;
  uint32_t fs;
  uint32_t gs;
  // instruction pointer
  uint32_t cs;
  uint32_t ss;
  uint64_t rip;
  // stack
  uint64_t rsp;
  uint64_t rbp;
  // flags

  uint64_t err_code;
  uint64_t isr_num; //48

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
    uint64_t *stack = (uint64_t *)rsp;
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
