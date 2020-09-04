#ifndef TASKING_H
#define TASKING_H

#include <Types.hpp>
#include <Exec/x86/bochs.hpp>

typedef struct {
  TUint64 task;
  TUint64 rip;
  TUint64 rflags;
  TUint64 ksp;
  TUint64 rsp;
  TUint64 cr3;
  TUint64 err_code;
  TUint64 isr_num;
  TUint8 fxsave[512+16];

  void Dump() {
    extern char *isr_names[];
    extern void dlog(const char *fmt, ...);
    dlog("task @ %x\n", this);
    dlog("isr_num: (%s) %x\n", "ISR", isr_num);
    dlog("err_code: %x\n", err_code);
    dlog("  stack:\n");
    TUint64 *stack = (TUint64 *)rsp;
    for (int i=0; i<10; i++) {
      dlog("    %016x: %016x\n", i, *stack++);
    }
    dlog("\n");
  }

} PACKED TTaskContext;

class CPU;
class BTask;

typedef struct {
  TUint64 mCurrentGs;
  BTask *mCurrentTask;
  CPU *mCurrentCpu;
} PACKED TGS;


#define USER_GS_BASE   0xc0000101
#define KERNEL_GS_BASE 0xc0000102


extern "C" void write_msr(TUint64 aRegister, TUint64 aValue);
extern "C" TUint64 read_msr(TUint64 aRegister);
extern "C" void swapgs();

extern "C" void SetCurrentTask(TTaskContext *aTask);
extern "C" TTaskContext *GetCurrentTask();

extern "C" void SetGS(TGS *aGsValue);
extern "C" TGS *GetGS();

extern "C" void SetCPU(CPU *aCpu);
extern "C" CPU *GetCPU();

#endif
