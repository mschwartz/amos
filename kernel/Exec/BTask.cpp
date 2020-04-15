#include <Exec/BTask.h>
#include <x86/bochs.h>

extern "C" TUint32 GetCS(), GetDS(), GetES(), GetFS(), GetGS();

BTask::BTask(const char *aName, TInt64 aPri, TUint64 aStackSize) : BNodePri(aName, aPri) {
  dprint("Construct BTask %s\n", aName);
  dputs(aName);
  dprint("---\n");
//#if 0
  task_t *regs = &mTaskX64;

  SetMemory8(regs, 0, sizeof(task_t));
  // initialize stack
  TUint8 *stack = (TUint8 *)AllocMem(aStackSize, MEMF_PUBLIC);
  mUpperSP = &stack[aStackSize];
  mLowerSP = &stack[0];

  regs->rsp = (TUint64)&stack[aStackSize];
  regs->rbp = regs->rsp;
  regs->rdi = (TUint64)this;
  regs->rip = (TUint64)this->RunWrapper;
  regs->cs = GetCS();
  regs->ds = GetDS();
  regs->es = GetCS();
  regs->fs = GetFS();
  regs->gs = GetGS();
//#endif 
}

BTask::~BTask() {
  //
}

void BTask::DumpRegisters(task_t *regs) {
  dprint("   isr_num %d\n", regs->isr_num);
  dprint("  err_code 0x%x\n", regs->err_code);
  dprint("        cs 0x%x\n", regs->cs);
  dprint("        ds 0x%x\n", regs->ds);
  dprint("        es 0x%x\n", regs->es);
  dprint("        fs 0x%x\n", regs->fs);
  dprint("        gs 0x%x\n", regs->gs);

  dprint("       rip 0x%x\n", regs->rip);
  dprint("        ss 0x%x rsp 0x%x rbp 0x%x\n", regs->ss, regs->rsp, regs->rbp);

  dprint("       rax 0x%x\n", regs->rax);
  dprint("       rbx 0x%x\n", regs->rbx);
  dprint("       rcx 0x%x\n", regs->rcx);
  dprint("       rdx 0x%x\n", regs->rdx);
  dprint("       rsi 0x%x\n", regs->rsi);
  dprint("       rdi 0x%x\n", regs->rdi);
}

void BTask::Dump() {
  task_t *regs = &mTaskX64;
  dprint("\nTask %s 0x%x regs 0x%x\n", mNodeName, this, regs);
  dputs(mNodeName);
  dprintf("---\n");
  DumpRegisters(regs);
}

