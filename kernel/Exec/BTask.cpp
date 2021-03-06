#include <Exec/ExecBase.hpp>
#include <Exec/BTask.hpp>
#include <Exec/CPU.hpp>
#include <Devices/TimerDevice.hpp>
#include <Devices/RtcDevice.hpp>

#define DEBUGME
#undef DEBUGME

extern "C" TUint32 GetCS(), GetDS(), GetES(), GetSS(), GetRFLAGS();
extern "C" void init_task_state(TTaskRegisters *t);

BTask::BTask(const char *aName, TInt64 aPri, TUint64 aStackSize)
    : BNodePri(aName, aPri), mInspirationBase(*gExecBase.GetInspirationBase()) {
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
  regs->upper_sp = (TUint64)&stack[aStackSize];
  regs->lower_sp = (TUint64)&stack[0];

  regs->rsp = (TUint64)regs->upper_sp;
  regs->rbp = regs->rsp;
  regs->ss = GetSS();
  regs->rdi = (TUint64)this;
  regs->rip = (TUint64)this->RunWrapper;
  regs->rax = (TUint64)this;
  regs->cs = 0x8;
  regs->ds = 0x10;
  regs->es = 0x10;
  // regs->fs = GetFS();
  // regs->gs = GetGS();
  regs->rflags = 0x202;
  init_task_state(regs);
}

BTask::~BTask() {
  //
}

void BTask::Suicide(TInt64 aCode) {
  // remove and delete me
  gExecBase.RemoveTask(this, aCode, ETrue);
}

void BTask::RunWrapper(BTask *aTask) {
  // CPU *c = (CPU *)aTask->mCpu;
  // BTask *t = ((CPU *)c)->CurrentTask();

  TInt64 code = aTask->Run();

  // if task returns it is removed from the active list and deleted.
  dlog("*** Task %s exited code(%d)\n", aTask->TaskName(), code);
  aTask->Suicide(code);
}

static void print_flag(TUint64 flags, TInt bit, const char *m) {
  if (flags & (1 << bit)) {
    dprint("%s ", m);
  }
}

void BTask::DumpRegisters(TTaskRegisters *regs) {
  DISABLE;

  dlog("   ===  isr_num %d err_code %d\n", regs->isr_num, regs->err_code);
  dlog("   rip: %016x cs: %08x\n", regs->rip, regs->cs);

  // print flags
  TUint64 f = regs->rflags;
  dlog(" flags: %016x ", f);
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

  // general purpose registeers
  dlog("   rax: %016x\n", regs->rax);
  dlog("   rbx: %016x\n", regs->rbx);
  dlog("   rcx: %016x\n", regs->rcx);
  dlog("   rdx: %016x\n", regs->rdx);
  dlog("   rsi: %016x\n", regs->rsi);
  dlog("   rdi: %016x\n", regs->rdi);
  dlog("    ds: %08x es: %08x \n", regs->ds, regs->es);
  dlog("    ss %08x rsp %016x rbp %016x\n", regs->ss, regs->rsp, regs->rbp);

  ENABLE;
}

void BTask::Dump() {
  DISABLE;
  TTaskRegisters *regs = &mRegisters;
  dlog("\nTask Dump %016x regs(%x)--- %s ---\n", this, &this->mRegisters, mNodeName);
  DumpRegisters(regs);
  dlog("  STACK:\n");
  TUint64 *addr = (TUint64 *)regs->rsp;
  for (TInt i = 0; i < 8; i++) {
    if ((TUint64)addr > mRegisters.upper_sp) {
      break;
    }
    dlog("  %016x: %016x\n", addr, *addr);
    addr++;
  }
  dprint("\n\n");
  ENABLE;
}

TInt8 BTask::AllocSignal(TInt64 aSignalNum) {
  DISABLE;

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

  ENABLE;
  return sig == 64 ? -1 : sig;
}

TBool BTask::FreeSignal(TInt64 aSignalNum) {
  DISABLE;
  TUint64 mask = 1 << aSignalNum;
  if (mSigAlloc & mask) {
    mSigAlloc &= ~mask;
    ENABLE;
    return ETrue;
  }
  ENABLE;
  return EFalse;
}

extern "C" void push_disable();
extern "C" void pop_disable();

void BTask::Signal(TInt64 aSignalSet) {
  mSigReceived |= aSignalSet;
  // assure this task is in active list and potentially perform a task switch
  if (mSigReceived & mSigWait) {
    //    if (CompareStrings(this->TaskName(), "Timer Task")) {
    //    dlog("Wake(%s)\n", this->TaskName());
    //    }
    gExecBase.Wake(this);
  }
}

TUint64 BTask::Wait(TUint64 aSignalSet) {
  if (aSignalSet == 0) {
    gExecBase.Wake(this);
    return 0;
  }

  mSigWait |= aSignalSet;

  gExecBase.WaitSignal(this);

  TUint64 received = mSigReceived;
  mSigReceived = 0;

  return received;
}

MessagePort *BTask::CreatePort(const char *aName, TInt64 aPri) {
  DISABLE;
  TInt64 sig = AllocSignal(-1);
  MessagePort *p = new MessagePort(aName, this, sig, aPri);
  gExecBase.AddMessagePort(*p);
  ENABLE;
  return p;
}

void BTask::FreePort(MessagePort *aMessagePort) {
  DISABLE;

  FreeSignal(aMessagePort->SignalNumber());
  gExecBase.RemoveMessagePort(*aMessagePort);

  ENABLE;

  delete aMessagePort;
}

void BTask::WaitForPort(const char *aName) {
  for (;;) {
    MessagePort *p = gExecBase.FindMessagePort(aName);
    if (p) {
      return;
    }
  }
}
TUint64 BTask::WaitPorts(TUint64 aSigMask, ...) {
  va_list args;
  va_start(args, aSigMask);
  TUint64 sigmask = aSigMask;

  for (;;) {
    MessagePort *p = va_arg(args, MessagePort *);
    if (p == ENull) {
      break;
    }
    sigmask |= 1 << p->SignalNumber();
  }
  va_end(args);
  return Wait(sigmask);
}

TUint64 BTask::WaitPort(MessagePort *aMessagePort, TUint64 aSignalMask) {
  return Wait(aSignalMask | (1 << aMessagePort->SignalNumber()));
}

TUint64 BTask::WaitPort(MessagePort *aMessagePort, MessagePort *aOtherPort, TUint64 aSignalMask) {
  return Wait(aSignalMask | (1 << aMessagePort->SignalNumber()) | (1 << aOtherPort->SignalNumber()));
}

TUint64 BTask::WaitPort(MessagePort *aMessagePort, MessagePort *aOtherPort, MessagePort *aOtherOtherPort, TUint64 aSignalMask) {
  return Wait(aSignalMask |
              (1 << aMessagePort->SignalNumber()) |
              (1 << aOtherPort->SignalNumber())) |
         (1 << aOtherOtherPort->SignalNumber());
}

void BTask::Sleep(TUint64 aSeconds) {
  MessagePort *timer = gExecBase.FindMessagePort("timer.device");
  if (!timer) {
    dlog("no timer port\n");
    return;
  }

  MessagePort *replyPort = CreatePort();
  TimerMessage *m = new TimerMessage(replyPort, ETimerSleep);
  m->mArg1 = aSeconds;
  m->Send(timer);
  WaitPort(replyPort);
  while ((m = (TimerMessage *)replyPort->GetMessage())) {
    delete m;
  }

  FreePort(replyPort);
}

void BTask::MilliSleep(TUint64 aMilliSeconds) {
  MessagePort *rtc = gExecBase.FindMessagePort("rtc.device");
  if (!rtc) {
    dlog("no rtc port\n");
    return;
  }

  MessagePort *replyPort = CreatePort();
  RtcMessage *m = new RtcMessage(replyPort, ERtcSleep);
  m->mArg1 = aMilliSeconds;
  m->Send(rtc);
  WaitPort(replyPort);
  while ((m = (RtcMessage *)replyPort->GetMessage())) {
    delete m;
  }

  FreePort(replyPort);
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
  // needs to be atomic
  DISABLE;
  mForbidNestCount++;
  ENABLE;
}

void BTask::Permit() {
  // needs to be atomic
  DISABLE;
  mForbidNestCount--;
  if (mForbidNestCount < 0) {
    mForbidNestCount = 0;
  }
  ENABLE;
}

void BTaskList::Dump() {
  DISABLE;
  BListPri::Dump();
  dprint("\n");
  ENABLE;
}
