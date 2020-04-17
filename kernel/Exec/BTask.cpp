#include <Exec/BTask.h>
#include <x86/bochs.h>
#include <Exec/ExecBase.h>

extern "C" TUint32 GetCS(), GetDS(), GetES(), GetFS(), GetGS(), GetSS(), GetRFLAGS();

BTask::BTask(const char *aName, TInt64 aPri, TUint64 aStackSize) : BNodePri(aName, aPri) {
  // initialize Forbid/Permit
  mForbidNestCount = 0;

  // initialize task's Signals
  mSigAlloc = 0;
  mSigWait = 0;
  mSigReceived = 0;

  // initialize task's registers
  task_t *regs = &mTaskX64;
  // zero out registers
  SetMemory8(regs, 0, sizeof(task_t));

  // initialize stack
  TUint8 *stack = (TUint8 *)AllocMem(aStackSize, MEMF_PUBLIC);
  mUpperSP = &stack[aStackSize];
  mLowerSP = &stack[0];

  regs->rsp = (TUint64)mUpperSP;
  regs->rbp = regs->rsp;
  regs->ss = GetSS();
  regs->rdi = (TUint64)this;
  regs->rip = (TUint64)this->RunWrapper;
  regs->rax = (TUint64)this;
  regs->cs = GetCS();
  regs->ds = GetDS();
  regs->es = GetES();
  regs->fs = GetFS();
  regs->gs = GetGS();
  regs->rflags = 0x202; // GetRFLAGS();
}

BTask::~BTask() {
  //
}

void BTask::RunWrapper(BTask *aTask) {
  BTask *t = gExecBase.GetCurrentTask();
  t->Run();
}

void BTask::DumpRegisters(task_t *regs) {
  dprint("   isr_num %d\n", regs->isr_num);
  dprint("  err_code 0x%x\n", regs->err_code);
  dprint("     flags 0x%x\n", regs->rflags);
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

TInt8 BTask::AllocSignal(TInt64 aSignalNum) {
  if (aSignalNum == -1) {
    for (TInt sig = 0; sig < 64; sig++) {
      TUint64 mask = 1 << sig;
      if ((mSigAlloc & mask) == 0) {
        mSigAlloc |= mask;
        return sig;
      }
    }
  }
  else if (aSignalNum >= 0) {
    TUint64 mask = 1 << aSignalNum;
    if ((mSigAlloc & mask) == 0) {
      mSigAlloc |= mask;
      return aSignalNum;
    }
  }
  return -1;
}

TBool BTask::FreeSignal(TInt64 aSignalNum) {
  TUint64 mask = 1 << aSignalNum;
  if (mSigAlloc & mask) {
    mSigAlloc &= ~mask;
    return ETrue;
  }
  return EFalse;
}

void BTask::Signal(TInt64 aSignalNum) {
  mSigReceived |= (1 << aSignalNum);
  gExecBase.Wake(this);
}

TUint64 BTask::Wait(TUint64 aSignalSet) {
  mSigWait |= aSignalSet;
  gExecBase.WaitSignal(this);

  gExecBase.Disable();
  TUint64 received = mSigReceived;
  mSigReceived = 0;
  gExecBase.Enable();

  return received;
}

void BTask::Disable() {
  gExecBase.Disable();
}
void BTask::Enable() {
  gExecBase.Disable();
}

void BTask::Forbid() {
  Disable();
  mForbidNestCount++;
  Enable();
}

void BTask::Permit() {
  Disable();
  mForbidNestCount--;
  if (mForbidNestCount < 0) {
    mForbidNestCount = 0;
  }
  Enable();
}
