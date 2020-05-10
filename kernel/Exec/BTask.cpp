#include <Exec/ExecBase.h>
#include <Exec/BTask.h>
#include <Devices/TimerDevice.h>

#define DEBUGME
#undef DEBUGME

extern "C" TUint32 GetCS(), GetDS(), GetES(), GetFS(), GetGS(), GetSS(), GetRFLAGS();

BTask::BTask(const char *aName, TInt64 aPri, TUint64 aStackSize) : BNodePri(aName, aPri), mInspirationBase(*gExecBase.GetInspirationBase()) {
  // initialize Forbid/Permit
  mForbidNestCount = 0;
  mDisableNestCount = 0;
  mTaskState = ETaskRunning;

  // initialize task's Signals
  mSigAlloc = 0;
  mSigWait = 0;
  mSigReceived = 0;

  // initialize task's registers
  TTaskRegisters *regs = &mRegisters;
  // zero out registers
  SetMemory8(regs, 0, sizeof(TTaskRegisters));

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
  t->Remove();
  delete t;
}

static void print_flag(TUint64 flags, TInt bit, const char *m) {
  if (flags & (1 << bit)) {
    dprint("%s ", m);
  }
}

void BTask::DumpRegisters(TTaskRegisters *regs) {
  TUint64 flags = GetFlags();
  cli();
  dlog("===  isr_num %d err_code %d\n", regs->isr_num, regs->err_code);
  // print flags
  TUint64 f = regs->rflags;
  dprint("   flags: %016x ", f);
  print_flag(f, 21, "ID ");
  print_flag(f, 20, "VIP ");
  print_flag(f, 19, "VIF ");
  print_flag(f, 18, "AC ");
  print_flag(f, 17, "VM ");
  print_flag(f, 16, "RF ");
  print_flag(f, 14, "NT ");
  dprint("IOPL(%d) ", (f >> 12) & 3);
  print_flag(f, 11, "OF ");
  print_flag(f, 10, "DF ");
  dprint("IF(%s) ", (f & (1 << 9)) ? "STI" : "CLI");
  print_flag(f, 8, "TF ");
  print_flag(f, 7, "SF ");
  print_flag(f, 6, "ZF ");
  print_flag(f, 4, "AF ");
  print_flag(f, 2, "PF ");
  print_flag(f, 0, "CF ");
  dprint("\n");

  dlog("   rax: %016x\n", regs->rax);
  dlog("   rbx: %016x\n", regs->rbx);
  dlog("   rcx: %016x\n", regs->rcx);
  dlog("   rdx: %016x\n", regs->rdx);
  dlog("   rsi: %016x\n", regs->rsi);
  dlog("   rdi: %016x\n", regs->rdi);
  dlog("    ds: %08x es: %08x fs: %08x gs: %08x\n", regs->ds, regs->es, regs->fs, regs->gs);
  dlog("    ss %08x rsp %016x rbp %016x\n", regs->ss, regs->rsp, regs->rbp);
  dlog("    cs: %08x rip: %016x\n", regs->cs, regs->cs);
#if 0
  dlog("rax 0x%x rbx 0x%x rcx 0x%x rdx 0x%x\n", regs->rax, regs->rbx, regs->rcx, regs->rdx);
  dlog("rsi %0x%x rdi %0x%x\n", regs->rsi, regs->rdi);
  dlog("cs 0x%x ds 0x%x es 0x%x fs 0x%x gs 0x%x\n", regs->cs, regs->ds, regs->es, regs->fs, regs->gs);
  dlog("ss 0x%x rsp 0x%x rbp 0x%x\n", regs->ss, regs->rsp, regs->rbp);
  dlog("rip 0x%x flags 0x%x\n", regs->rip, regs->rflags);
#endif
  SetFlags(flags);
}

void BTask::Dump() {
  TUint64 flags = GetFlags();
  cli();
  TTaskRegisters *regs = &mRegisters;
  dlog("\nTask Dump %016x --- %s ---\n", this, mNodeName);
  DumpRegisters(regs);
  dlog("  STACK:\n");
  TUint64 *addr = (TUint64 *)regs->rsp;
  for (TInt i = 0; i < 10; i++) {
    if (addr > mUpperSP) {
      break;
    }
    dlog("  %016x: %016x\n", addr, *addr);
    addr++;
  }
  dprint("\n\n");
  SetFlags(flags);
}

TInt8 BTask::AllocSignal(TInt64 aSignalNum) {
  TUint64 flags = GetFlags();
  cli();

  TInt8 sig = -1;
  if (aSignalNum == -1) {
    for (sig = 0; sig < 64; sig++) {
      TUint64 mask = 1 << sig;
      if ((mSigAlloc & mask) == 0) {
        mSigAlloc |= mask;
        break;
      }
    }
  }
  else if (aSignalNum >= 0) {
    TUint64 mask = 1 << aSignalNum;
    if ((mSigAlloc & mask) == 0) {
      mSigAlloc |= mask;
      sig = aSignalNum;
    }
  }

  SetFlags(flags);
  return sig == 64 ? -1 : sig;
}

TBool BTask::FreeSignal(TInt64 aSignalNum) {
  TUint64 flags = GetFlags();
  cli();
  TUint64 mask = 1 << aSignalNum;
  if (mSigAlloc & mask) {
    mSigAlloc &= ~mask;
    SetFlags(flags);
    return ETrue;
  }
  SetFlags(flags);
  return EFalse;
}

extern "C" void push_disable();
extern "C" void pop_disable();

void BTask::Signal(TInt64 aSignalSet) {
  mSigReceived |= aSignalSet;
  // assure this task is in active list and potentially perform a task switch
  if (mSigReceived & mSigWait) {
    gExecBase.Wake(this);
  }
}

TUint64 BTask::Wait(TUint64 aSignalSet) {
  if (aSignalSet == 0) {
    gExecBase.Wake(this);
    return 0;
  }
  TUint64 flags = GetFlags();
  cli();

  mSigWait |= aSignalSet;

  SetFlags(flags);
  gExecBase.WaitSignal(this);

  flags = GetFlags();
  volatile TUint64 received = mSigReceived;
  mSigReceived = 0;

  SetFlags(flags);
  return received;
}

MessagePort *BTask::CreateMessagePort(const char *aName, TInt64 aPri) {
  TUint64 flags = GetFlags();
  cli();
  TInt64 sig = AllocSignal(-1);
  MessagePort *p = new MessagePort(aName, this, sig, aPri);
  gExecBase.AddMessagePort(*p);
  SetFlags(flags);
  return p;
}

void BTask::FreeMessagePort(MessagePort *aMessagePort) {
  TUint64 flags = GetFlags();
  cli();
  FreeSignal(aMessagePort->SignalNumber());
  gExecBase.RemoveMessagePort(*aMessagePort);
  SetFlags(flags);
  delete aMessagePort;
}

TUint64 BTask::WaitPort(MessagePort *aMessagePort) {
  return Wait(1 << aMessagePort->SignalNumber());
}

void BTask::Sleep(TUint64 aSeconds) {
  MessagePort *timer = gExecBase.FindMessagePort("timer.device");
  while (!timer) {
    dlog("***** can't find timer.device port\n");
    timer = gExecBase.FindMessagePort("timer.device");
  }
  MessagePort *replyPort = CreateMessagePort();
  TimerMessage *m = new TimerMessage(replyPort, ETimerSleep);
  m->mArg1 = aSeconds;
  m->SendMessage(timer);
  WaitPort(replyPort);
  while ((m = (TimerMessage *)replyPort->GetMessage())) {
    delete m;
  }
  FreeMessagePort(replyPort);
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
  TUint64 flags = GetFlags();
  cli();
  mForbidNestCount++;
  SetFlags(flags);
}

void BTask::Permit() {
  TUint64 flags = GetFlags();
  cli();
  mForbidNestCount--;
  if (mForbidNestCount < 0) {
    mForbidNestCount = 0;
  }
  SetFlags(flags);
}

void BTaskList::Dump() {
  TUint64 flags = GetFlags();
  cli();
  BListPri::Dump();
  dprint("\n");
  SetFlags(flags);
}
