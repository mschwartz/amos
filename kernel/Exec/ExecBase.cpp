#include <Exec/ExecBase.h>
#include <x86/bochs.h>
#include <stdint.h>

#include <Devices/KeyboardDevice.h>
#include <Devices/TimerDevice.h>
#include <Devices/RtcDevice.h>

#include <posix/sprintf.h>


ExecBase gExecBase;

extern "C" void schedule_trap();
extern "C" void eputs(const char *s);

extern "C" void enter_next_task();

class IdleTask : public BTask {
public:
  IdleTask() : BTask("Idle Task", INT64_MAX) {}

public:
  void Run() {
    //    sti();
    dlog("IdleTask Running\n");
    while (1) {
      halt();
    }
  }
};

typedef struct {
  TUint16 mMode;
  TUint16 mWidth;
  TUint16 mHeight;
  TUint16 mPitch;
  TUint16 mBitsPerPixel;
  TUint16 mPad;
  void Dump() {
    dlog("Mode %x %d x %d %d bpp\n", mMode, mWidth, mHeight, mBitsPerPixel);
  }
} PACKED TModeInfo;

typedef struct {
  TInt16 mCount;          // number of modes found
  TModeInfo mDisplayMode; // chosen display mode
  TModeInfo mModes[];
  void Dump() {
    dlog("Found %d %x modes\n", mCount, mCount);
    for (TInt16 i = 0; i < mCount; i++) {
      mModes[i].Dump();
    }
  }
} PACKED TModes;

ExecBase::ExecBase() {
  in_bochs = *((TUint8 *)0x7c10);
//  dlog("bochs %x\n", in_bochs);

  dlog("ExecBase constructor called\n");

  // set up paging
  mMMU = new MMU;
  dlog("  initialized MMU\n");

  extern void *init_start, *init_end,
    *text_start, *text_end,
    *rodata_start, *rodata_end,
    *data_start, *data_end,
    *bss_start, *bss_end,
    *kernel_end;
  dlog("Amigo V1.0\n");
  dlog("         init: %016x - %016x\n", &init_start, &init_end);
  dlog("         text: %016x - %016x\n", &text_start, &text_end);
  dlog("       rodata: %016x - %016x\n", &rodata_start, &rodata_end);
  dlog("         data: %016x - %016x\n", &data_start, &data_end);
  dlog("          bss: %016x - %016x\n", &bss_start, &bss_end);
  dlog("   kernel_end: %016x\n", &kernel_end);
  dlog("system memory: %d (%d pages)\n", mMMU->total_memory(), mMMU->total_pages());

  TModes *modes = (TModes *)0x5000;
  dlog("\n\nDisplay Mode:\n");
  modes->mDisplayMode.Dump();
//  gDeviceList.FindDevice("FOO>DEVICE");
  mMessagePortList = new BMessagePortList("ExecBase MessagePort List");

  //  Screen s;
  mScreen = new Screen;
  dlog("  initialized screen\n");

  mGDT = new GDT;
  dlog("  initialized GDT\n");

  mIDT = new IDT;
  dlog("  initialized IDT\n");

  InitInterrupts();

  // Before enabling interrupts, we need to have the idle task set up
  IdleTask *task = new IdleTask();
  mActiveTasks.Add(*task);
  mCurrentTask = mActiveTasks.First();
  current_task = ENull;
  next_task = &mCurrentTask->mRegisters;

  // set up 8259 PIC
  gPIC = new PIC;
  mDisableNestCount = 0;
  sti();
  Disable();
  dlog("  initialized 8259 PIC\n");
  Enable();


  // initialize devices
  AddDevice(mTimer = new TimerDevice());
  dlog("  initialized timer\n");

  AddDevice(mRtc = new RtcDevice());
  dlog("  initialized rtc \n");

  AddDevice(new KeyboardDevice);
  dlog("  initialized keyboard \n");
}

ExecBase::~ExecBase() {
  dlog("ExecBase destructor called\n");
}

TUint64 ExecBase::SystemTicks() {
  return mRtc->mMillis;
//  return mTimer->GetTicks();
}

void ExecBase::Disable() {
  if (mDisableNestCount++ == 0) {
    cli();
  }
}

void ExecBase::Enable() {
  if (--mDisableNestCount <= 0) {
    mDisableNestCount = 0;
    sti();
  }
}

void ExecBase::AddInterruptHandler(TUint8 aIndex, TInterruptHandler *aHandler, TAny *aData, const char *aDescription) {
  mIDT->install_handler(aIndex, aHandler, aData, aDescription);
}

void ExecBase::putc(char c) {
  mScreen->putc(c);
}

void ExecBase::puts(const char *s) {
  mScreen->puts(s);
}

void ExecBase::newline() {
  mScreen->newline();
}

void ExecBase::AddTask(BTask *aTask) {
  TUint64 flags = GetFlags();
  cli();
  dlog("Add Task %016x --- %s --- rip=%016x rsp=%016x\n", aTask, aTask->mNodeName, aTask->mRegisters.rip, aTask->mRegisters.rsp);
  mActiveTasks.Add(*aTask);
  SetFlags(flags);
}

void ExecBase::WaitSignal(BTask *aTask) {
  cli();
  aTask->Remove();
  // If task has received a signal it's waiting for, we don't want to move it to the WAIT list,
  // but it may be lower priority than another task so we need to sort it in to ACTIVE list.
  if (aTask->mSigWait & aTask->mSigReceived) {
    dputs("\n");
    mActiveTasks.Add(*aTask);
    aTask->mTaskState = ETaskRunning;
  }
  else {
    mWaitingTasks.Add(*aTask);
    aTask->mTaskState = ETaskWaiting;
  }
  Schedule();
}

/**
  * Wake() - if task is on Wait list, move it to active list.  If already on active list, re-add it.
  * Note that Add() will sort the task into the list, effecting round-robin order.
  */
void ExecBase::Wake(BTask *aTask) {
  // note that removing and adding the task will sort the task at the end of all tasks with the same priority.
  // this effects round-robin.
  aTask->Remove();
  mActiveTasks.Add(*aTask);
  aTask->mTaskState = ETaskRunning;
}

void ExecBase::Schedule() {
  schedule_trap();
}

void ExecBase::Kickstart() {
  enter_next_task(); // just enter next task
}

/**
 * Determine next task to run.  This should only be called from IRQ/Interrupt context with interrupts disabled.
 */
void ExecBase::RescheduleIRQ() {
  cli();
  if (mCurrentTask) {
    if (mCurrentTask->mForbidNestCount == 0) {
      mCurrentTask->Remove();
      if (mCurrentTask->mTaskState == ETaskWaiting) {
        mWaitingTasks.Add(*mCurrentTask);
      }
      else {
        mActiveTasks.Add(*mCurrentTask);
      }
    }
  }
  mCurrentTask = mActiveTasks.First();
  current_task = &mCurrentTask->mRegisters;
}

void ExecBase::AddMessagePort(BMessagePort &aMessagePort) {
  mMessagePortList->Add(aMessagePort);
}

TBool ExecBase::RemoveMessagePort(BMessagePort &aMessagePort) {
  if (mMessagePortList->Find(aMessagePort)) {
    aMessagePort.Remove();
    return ETrue;
  }
  return EFalse;
}

void ExecBase::GuruMeditation(const char *aFormat, ...) {
  cli();
  char buf[512];
  dlog("\n\n***********************\n");
  dlog("GURU MEDITATION at %dms\n", SystemTicks());
  va_list args;
  va_start(args, aFormat);
  vsprintf(buf, aFormat, args);
  dlog(buf);

  mCurrentTask->Dump();
  va_end(args);
  dlog("***********************\n\n\nHalted.\n");
  while (1) {
    halt();
  }
}

void ExecBase::AddDevice(BDevice *aDevice) {
  mDeviceList.Add(*aDevice);
}

BDevice *ExecBase::FindDevice(const char *aName) {
  return mDeviceList.FindDevice(aName);
}

class DefaultException : public BInterrupt {
public:
  DefaultException(const char *aKind) : BInterrupt(aKind, LIST_PRI_MIN) {}
  ~DefaultException();

public:
  TBool Run(TAny *aData) {
    char buf[128];
    cli();
    sprintf(buf, "%s Exception", mNodeName);
    gExecBase.GuruMeditation(buf);
    // TODO: kill/remove current task
    halt();
    return ETrue;
  }
};

class DefaultIRQ : public BInterrupt {
public:
  DefaultIRQ(const char *aKind) : BInterrupt(aKind, LIST_PRI_MIN) {}
  ~DefaultIRQ();

public:
  TBool Run(TAny *aData) {
    dlog("%s IRQ\n", mNodeName);
    return ETrue;
  }
};

class NextTaskTrap : public BInterrupt {
public:
  NextTaskTrap(const char *aKind) : BInterrupt(aKind, LIST_PRI_MIN) {}
  ~NextTaskTrap();

public:
  TBool Run(TAny *aData) {
    // at this point current_task is saved
    cli();
    gExecBase.RescheduleIRQ();
    return ETrue;
  }
};

void ExecBase::SetIntVector(EInterruptNumber aInterruptNumber, BInterrupt *aInterrupt) {
  mInterrupts[aInterruptNumber].Add(*aInterrupt);
}

extern "C" TUint64 GetRFLAGS();

TBool ExecBase::RootHandler(TInt64 aInterruptNumber, TAny *aData) {
  cli();
  BInterruptList *list = &gExecBase.mInterrupts[aInterruptNumber];
  for (BInterrupt *i = (BInterrupt *)list->First(); !list->End(i); i = (BInterrupt *)i->mNext) {
    if (i->Run(aData)) {
      return ETrue;
    }
  }
  // TODO: no handler!
  dlog("No handler!\n");
  return EFalse;
}

void ExecBase::SetException(EInterruptNumber aIndex, const char *aName) {
  TUint64 flags = GetFlags();
  cli();
  IDT::install_handler(aIndex, ExecBase::RootHandler, this, aName);
  SetIntVector(aIndex, new DefaultException(aName));
  SetFlags(flags);
}

void ExecBase::SetInterrupt(EInterruptNumber aIndex, const char *aName) {
  TUint64 flags = GetFlags();
  cli();
  IDT::install_handler(aIndex, ExecBase::RootHandler, this, aName);
  SetIntVector(aIndex, new DefaultIRQ(aName));
  SetFlags(flags);
}

void ExecBase::SetTrap(EInterruptNumber aIndex, const char *aName) {
  TUint64 flags = GetFlags();
  cli();
  dlog("Add Trap %d %s\n", aIndex, aName);
  IDT::install_handler(aIndex, ExecBase::RootHandler, this, aName);
  SetIntVector(aIndex, new NextTaskTrap(aName));
  SetFlags(flags);
}

void ExecBase::InitInterrupts() {
  TUint64 flags = GetFlags();
  cli();
  // install default exception handlers
  SetException(EDivideError, "Divide Error");
  SetException(EDebug, "Debug");
  SetException(ENmi, "NMI");
  SetException(EInt3, "Int3");
  SetException(EOverflow, "Overflow");
  SetException(EOutOfBounds, "OutOfBounds");
  SetException(EInvalidOpCode, "InvalidOpCode");
  SetException(EDeviceNotAvailable, "DeviceNotAvailable");
  SetException(EDoubleFault, "DoubleFault");
  SetException(ECoprocessorSegmentOverrun, "CoprocessorSegmentOverrun");
  SetException(EInvalidTSS, "InvalidTSS");
  SetException(ESegmentNotPresent, "SegmentNotPresent");
  SetException(EStackFault, "StackFault");
  SetException(EGeneralProtection, "GeneralProtection");
  SetException(EPageFault, "PageFault");
  SetException(EUnknown, "Unknown");
  SetException(ECoprocessorFault, "CoprocessorFault");
  SetException(EAlignmentCheck, "AlignmentCheck");
  SetException(EMachineCheck, "MachineCheck");
  SetException(ESimdFault, "SimdFault");
  SetException(EVirtualization, "Virtualization");
  SetException(ESecurity, "Security");
  //
  SetInterrupt(ETimerIRQ, "Timer");
  SetInterrupt(EKeyboardIRQ, "Keyboard");
  SetInterrupt(ESlavePicIRQ, "SlavePic");
  SetInterrupt(ECom2IRQ, "Com2");
  SetInterrupt(ECom1IRQ, "Com1");
  SetInterrupt(ELpt2IRQ, "Lpt2");
  SetInterrupt(EFloppyIRQ, "Floppy");
  SetInterrupt(ELpt1IRQ, "Lpt1");
  SetInterrupt(ERtcClockIRQ, "RtClock");
  SetInterrupt(EMasterPicIRQ, "MasterPic");
  SetInterrupt(EReserved1IRQ, "Reserved1");
  SetInterrupt(EReserved2IRQ, "Reserved2");
  SetInterrupt(EReserved3IRQ, "Reserved3");
  SetInterrupt(ECoprocessorIRQ, "Coprocessor");
  SetInterrupt(EHardDiskIRQ, "HardDisk");
  //  SetInterrupt(EReserved4IRQ, "Reserved4");
  SetTrap(ETrap0, "Trap0");

  SetFlags(flags);
}
