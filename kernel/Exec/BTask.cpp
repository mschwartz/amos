#include <Exec/BTask.h>
#include <x86/bochs.h>
#include <Exec/ExecBase.h>

#define DEBUGME
#undef DEBUGME

extern "C" TUint32 GetCS(), GetDS(), GetES(), GetFS(), GetGS(), GetSS(), GetRFLAGS();

BTask::BTask(const char *aName, TInt64 aPri, TUint64 aStackSize) : BNodePri(aName, aPri) {
  // initialize Forbid/Permit
  mForbidNestCount = 0;
  mDisableNestCount = 0;
  mTaskState = ETaskRunning;

  // initialize task's Signals
  mSigAlloc = 0;
  mSigWait = 0;
  mSigReceived = 0;

  // initialize task's registers
  task_t *regs = &mRegisters;
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
  Disable();
  dprint("isr_num %d err_code %d\n", regs->isr_num, regs->err_code);
  dprint("rax 0x%x rbx 0x%x rcx 0x%x rdx 0x%x\n", regs->rax, regs->rbx, regs->rcx, regs->rdx);
  dprint("rsi %0x%x rdi %0x%x\n", regs->rsi, regs->rdi);
  dprint("cs 0x%x ds 0x%x es 0x%x fs 0x%x gs 0x%x\n", regs->cs, regs->ds, regs->es, regs->fs, regs->gs);
  dprint("ss 0x%x rsp 0x%x rbp 0x%x\n", regs->ss, regs->rsp, regs->rbp);
  dprint("rip 0x%x flags 0x%x\n", regs->rip, regs->rflags);
  Enable();
}

void BTask::Dump() {
  Disable();
  task_t *regs = &mRegisters;
  dprint("\nTask 0x%x --- ", this); 
  dputs(mNodeName);
  dprintf( "---\n");
  DumpRegisters(regs);
  dprint("  STACK:\n");
  TUint64 *addr = (TUint64 *)regs->rsp;
  for (TInt i = 0; i < 10; i++) {
    if (addr > mUpperSP) {
      break;
    }
    dhex64((TUint64)addr);
    dprint(": ");
    dhex64(*addr++);
    dprint("\n");
  }
  Enable();
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

extern "C" void push_disable();
extern "C" void pop_disable();

void BTask::Signal(TInt64 aSignalSet) {
  mSigReceived |= aSignalSet;
#ifdef DEBUGME
  dprint("    BTASK Signal received %x\n", aSignalSet);
#endif
  // assure this task is in active list and potentially perform a task switch
  gExecBase.Wake(this);
}

TUint64 BTask::Wait(TUint64 aSignalSet) {
  gExecBase.Disable();
  mSigWait |= aSignalSet;

#ifdef DEBUGME
  dprint("About to WaitSignal\n");
#endif

  gExecBase.WaitSignal(this);

  volatile TUint64 received = mSigReceived;
  mSigReceived = 0;

#ifdef DEBUGME
  dprint("WaitSignal returned 0x%x\n", received);
#endif

  gExecBase.Enable();

  return received;
}

BMessagePort *BTask::CreateMessagePort(const char *aName, TInt64 aPri) {
  Disable();
  TInt64 sig = AllocSignal(-1);
  BMessagePort *p = new BMessagePort(aName, this, sig, aPri);
  gExecBase.AddMessagePort(*p);
  Enable();
  return p;
}

void BTask::FreeMessagePort(BMessagePort *aMessagePort) {
  Disable();
  FreeSignal(aMessagePort->SignalNumber());
  gExecBase.RemoveMessagePort(*aMessagePort);
  Enable();
  delete aMessagePort;
}

TUint64 BTask::WaitPort(BMessagePort *aMessagePort) {
  //  dprint("WaitPort %x %d\n", aMessagePort, aMessagePort->SignalNumber());
  //  return 1<<aMessagePort->SignalNumber();
  return Wait(1 << aMessagePort->SignalNumber());
}

void BTask::Disable() {
  gExecBase.Disable();
  if (++mDisableNestCount != 1) {
    gExecBase.Enable();
  }
}

void BTask::Enable() {
  gExecBase.Disable();
  if (--mDisableNestCount <= 0) {
    mDisableNestCount = 0;
    gExecBase.Enable();
  }
  gExecBase.Enable();
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
