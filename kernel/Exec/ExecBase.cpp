#include <Exec/ExecBase.h>
#include <Inspiration/InspirationBase.h>
#include <stdint.h>

#include <Exec/x86/mmu.h>
#include <Exec/x86/idt.h>
#include <Exec/x86/tss.h>
#include <Exec/x86/gdt.h>
#include <Exec/x86/cpu.h>
#include <Exec/x86/pic.h>
#include <Exec/x86/ps2.h>
#include <Exec/x86/pci.h>
#include <Exec/x86/acpi.h>

#include <Devices/AtaDevice.h>
#include <Devices/SerialDevice.h>
#include <Devices/KeyboardDevice.h>
#include <Devices/TimerDevice.h>
#include <Devices/RtcDevice.h>
#include <Devices/MouseDevice.h>

#include <FileSystems/SimpleFileSystem/SimpleFileSystem.h>

#include <posix/sprintf.h>
#include <Exec/Random.h>

ExecBase gExecBase;

extern "C" void schedule_trap();
extern "C" void eputs(const char *s);

extern "C" void enter_tasking();

class IdleTask : public BTask {
  friend ExecBase;

public:
  IdleTask() : BTask("Idle Task", LIST_PRI_MIN) {}

public:
  void Run() {
    dprint("\n");
    dlog("IdleTask Run\n");

    // initialize devices
    dlog("  initialize timer\n");
    gExecBase.AddDevice(new TimerDevice());

    // dlog("  initialize serial\n");
    // AddDevice(new SerialDevice());

    dlog("  initialize rtc \n");
    gExecBase.AddDevice(new RtcDevice());

    dlog("  initialize keyboard \n");
    gExecBase.AddDevice(new KeyboardDevice);

    dlog("  initialize mouse \n");
    gExecBase.AddDevice(new MouseDevice());

    dlog("  initialize ata disk \n");
    gExecBase.AddDevice(new AtaDevice(ENull));

    dlog("  initialize file system\n");
    gExecBase.AddFileSystem(new SimpleFileSystem("ata.device", 0, gSystemInfo.mRootSector));

    dlog("  initialize Inspiration\n");
    gExecBase.mInspirationBase = new InspirationBase();
    gExecBase.mInspirationBase->Init();

    while (1) {
      dlog("IdleTask Looping\n");
      halt();
    }
  }
};

typedef struct {
  //  TUint16 mPad0;
  TUint32 mMode;
  TUint32 mFrameBuffer;
  TUint32 mWidth;
  TUint32 mHeight;
  TUint32 mPitch;
  TUint32 mDepth;
  TUint32 mPlanes;
  TUint32 mBanks;
  TUint32 mBankSize;
  TUint32 mMemoryModel;
  TUint32 mFrameBufferOffset;
  TUint32 mFrameBufferSize;
  TUint32 mPad2;
  void Dump() {
    dlog("Mode(%x) mode(%x) dimensions(%dx%d) depth(%d)  pitch(%d) lfb(0x%x)\n",
      this, mMode, mWidth, mHeight, mDepth, mPitch, mFrameBuffer);
  }
} PACKED TModeInfo;

typedef struct {
  TInt32 mCount;          // number of modes found
  TModeInfo mDisplayMode; // chosen display mode
  TModeInfo mModes[];
  void Dump() {
    dlog("Found %d %x modes\n", mCount, mCount);
    for (TInt16 i = 0; i < mCount; i++) {
      mModes[i].Dump();
    }
  }
} PACKED TModes;

extern "C" TUint64 rdrand();

// ExecBase constructor
ExecBase::ExecBase() {
  dlog("ExecBase constructor called\n");

  TModes *modes = (TModes *)0xa000;
  // TModeInfo &i = modes->mDisplayMode;

  // TSystemInfo *bootInfo = (TSystemInfo *)0x5000;

  // // set up SystemInfo
  // mSystemInfo.mScreenWidth = i.mWidth;
  // mSystemInfo.mScreenHeight = i.mHeight;
  // mSystemInfo.mScreenDepth = i.mDepth;
  // mSystemInfo.mScreenPitch = i.mPitch;
  // TUint64 fb = (TUint64)i.mFrameBuffer;
  // mSystemInfo.mScreenFrameBuffer = (TAny *)fb;
  // mSystemInfo.mMillis = 0;

  //  SeedRandom(rdrand());
  SeedRandom64(1);

  dlog("\n\nDisplay Mode:\n");
  modes->mDisplayMode.Dump();

  // set up paging
  mMMU = new MMU;
  dlog("  initialized MMU\n");

  mMessagePortList = new MessagePortList("ExecBase MessagePort List");

  mTSS = new TSS;

  mGDT = new GDT(mTSS);
  // mGDT = new GDT();
  dlog("  initialized GDT\n");

  mIDT = new IDT;
  dlog("  initialized IDT\n");

  mPCI = new PCI();
  dlog("  initialized PCI\n");

  mACPI = new ACPI();
  dlog("  initialized ACPI\n");

  InitInterrupts();

  // set up 8259 PIC
  mPIC = new PIC;
  mDisableNestCount = 0;

  //  sti();
  Disable();
  dlog("  initialized 8259 PIC\n");

#ifdef ENABLE_PS2
  mPS2 = new PS2();
#else
  mPS2 = ENull;
#endif

  // Before enabling interrupts, we need to have the idle task set up
  IdleTask *task = new IdleTask();
  mActiveTasks.Add(*task);
  mCurrentTask = mActiveTasks.First();
  current_task = &mCurrentTask->mRegisters;

  Enable();
}

ExecBase::~ExecBase() {
  dlog("ExecBase destructor called\n");
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

//void ExecBase::AddInterruptHandler(TUint8 aIndex, TInterruptHandler *aHandler, TAny *aData, const char *aDescription) {
//  mIDT->InstallHandler(aIndex, aHandler, aData, aDescription);
//}

void ExecBase::AddTask(BTask *aTask) {
  TUint64 flags = GetFlags();
  cli();

  aTask->mRegisters.tss = (TUint64)&mTSS->mTss;
  dlog("    Add Task %016x --- %s --- rip=%016x rsp=%016x\n", aTask, aTask->mNodeName, aTask->mRegisters.rip, aTask->mRegisters.rsp);
  //  aTask->Dump();
  mActiveTasks.Add(*aTask);
  //  mActiveTasks.Dump();
  //  dlog("x\n");

  SetFlags(flags);
}

void ExecBase::DumpTasks() {
  dprint("\n\nActive Tasks\n");
  mActiveTasks.Dump();
  dprint("Waiting Tasks\n");
  mWaitingTasks.Dump();
  dprint("\n\n");
}

void ExecBase::WaitSignal(BTask *aTask) {
  DISABLE;
  aTask->Remove();
  // If task has received a signal it's waiting for, we don't want to move it to the WAIT list,
  // but it may be lower priority than another task so we need to sort it in to ACTIVE list.
  if (aTask->mSigWait & aTask->mSigReceived) {
    mActiveTasks.Add(*aTask);
    aTask->mTaskState = ETaskRunning;
  }
  else {
    mWaitingTasks.Add(*aTask);
    aTask->mTaskState = ETaskWaiting;
  }
  Schedule();
  ENABLE;
}

/**
  * Wake() - if task is on Wait list, move it to active list.  If already on active list, re-add it.
  * Note that Add() will sort the task into the list, effecting round-robin order.
  */
void ExecBase::Wake(BTask *aTask) {
  // note that removing and adding the task will sort the task at the end of all tasks with the same priority.
  // this effects round-robin.
  DISABLE;
  aTask->Remove();
  mActiveTasks.Add(*aTask);
  aTask->mTaskState = ETaskRunning;
  //  dlog("Wake %s\n", aTask->TaskName());
  ENABLE;
  //  DumpTasks();
}

void ExecBase::Schedule() {
  schedule_trap();
}

void ExecBase::Kickstart() {
  enter_tasking(); // just enter next task
}

/**
 * Determine next task to run.  This should only be called from IRQ/Interrupt context with interrupts disabled.
 */
void ExecBase::RescheduleIRQ() {
//  BTask *t = mCurrentTask;
//  cli();
#if 1
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
    // else {
    //   dlog("FORBID\n");
    // }
  }
#endif
  mCurrentTask = mActiveTasks.First();
  current_task = &mCurrentTask->mRegisters;
  //  if (t != mCurrentTask) {
  //    dprint("Reschedule %s(%x) %016x %x\n", mCurrentTask->TaskName(), mCurrentTask, current_task->rip, current_task->rflags);
  //    mCurrentTask->Dump();
  //    dprint("Previous task\n");
  //    t->Dump();
  //    dprint("\n\n\n");
  //  }
}

void ExecBase::AddMessagePort(MessagePort &aMessagePort) {
  DISABLE;
  mMessagePortList->Add(aMessagePort);
  ENABLE;
}

TBool ExecBase::RemoveMessagePort(MessagePort &aMessagePort) {
  if (mMessagePortList->Find(aMessagePort)) {
    DISABLE;
    aMessagePort.Remove();
    ENABLE;
    return ETrue;
  }
  return EFalse;
}

MessagePort *ExecBase::FindMessagePort(const char *aName) {
  DISABLE;
  MessagePort *mp = (MessagePort *)mMessagePortList->Find(aName);
  ENABLE;
  return mp;
}

void ExecBase::GuruMeditation(const char *aFormat, ...) {
  cli();
  bochs;
  char buf[512];
  dprint("\n\n***********************\n");
  dprint("GURU MEDITATION at %dms\n", SystemTicks());

  va_list args;
  va_start(args, aFormat);
  vsprintf(buf, aFormat, args);
  dprint(buf);
  dprint("\n");

  mCurrentTask->Dump();
  va_end(args);
  dprint("***********************\n\n\nHalted.\n");

  while (1) {
    halt();
  }
}

void ExecBase::AddDevice(BDevice *aDevice) {
  DISABLE;
  mDeviceList.Add(*aDevice);
  ENABLE;
}

BDevice *ExecBase::FindDevice(const char *aName) {
  DISABLE;
  BDevice *d = mDeviceList.FindDevice(aName);
  ENABLE;
  return d;
}

void ExecBase::AddFileSystem(BFileSystem *aFileSystem) {
  DISABLE;
  mFileSystemList.AddHead(*aFileSystem);
  ENABLE;
}

class DefaultException : public BInterrupt {
public:
  DefaultException(const char *aKind) : BInterrupt(aKind, LIST_PRI_MIN) {}
  ~DefaultException();

public:
  TBool Run(TAny *aData) {
    cli();
    gExecBase.GuruMeditation("%s Exception", mNodeName);
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
    gExecBase.RescheduleIRQ();
    return ETrue;
  }
};

void ExecBase::SetIntVector(EInterruptNumber aInterruptNumber, BInterrupt *aInterrupt) {
  mInterrupts[aInterruptNumber].Add(*aInterrupt);
}

void ExecBase::EnableIRQ(TUint16 aIRQ) {
  mPIC->EnableIRQ(aIRQ);
}

void ExecBase::DisableIRQ(TUint16 aIRQ) {
  mPIC->DisableIRQ(aIRQ);
}

void ExecBase::AckIRQ(TUint16 aIRQ) {
  mPIC->AckIRQ(aIRQ);
}

extern "C" TUint64 GetRFLAGS();

/**
 * RootHandler
 *
 * This is installed for all hardware trap, exception, and interrupt vectors.
 * 
 * For each of the above, there maty be a priority sorted linked list of handlers to be 
 * called to handle the trap/exception/interrupt.  If any of the handlers return ETrue,
 * no more handlers are called.  This allows the handler that handles the 
 * trap/exception/interrupt prevent other handlers from even running.
 *
 * Note that there may be multiple interrupts that fire a vector.
 */
TBool ExecBase::RootHandler(TInt64 aInterruptNumber, TAny *aData) {
  cli();
  BInterruptList *list = &gExecBase.mInterrupts[aInterruptNumber];
  for (BInterrupt *i = (BInterrupt *)list->First(); !list->End(i); i = (BInterrupt *)i->mNext) {
    if (i->Run(i->mData)) {
      return ETrue;
    }
  }
  // TODO: no handler!
  dlog("No handler!\n");
  return EFalse;
}

void ExecBase::SetException(EInterruptNumber aIndex, const char *aName) {
  DISABLE;
  IDT::InstallHandler(aIndex, ExecBase::RootHandler, this, aName);
  SetIntVector(aIndex, new DefaultException(aName));
  ENABLE;
}

void ExecBase::SetInterrupt(EInterruptNumber aIndex, const char *aName) {
  DISABLE;
  IDT::InstallHandler(aIndex, ExecBase::RootHandler, this, aName);
  SetIntVector(aIndex, new DefaultIRQ(aName));
  ENABLE;
}

void ExecBase::SetTrap(EInterruptNumber aIndex, const char *aName) {
  DISABLE;
  //  dlog("Add Trap %d %s\n", aIndex, aName);
  IDT::InstallHandler(aIndex, ExecBase::RootHandler, this, aName);
  SetIntVector(aIndex, new NextTaskTrap(aName));
  ENABLE;
}

void ExecBase::InitInterrupts() {
  DISABLE;
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
  SetInterrupt(EMouseIRQ, "Mouse");
  SetInterrupt(ECoprocessorIRQ, "Coprocessor");
  SetInterrupt(EAta1IRQ, "Ata 1");
  SetInterrupt(EAta2IRQ, "Ata 2");
  SetTrap(ETrap0, "Trap0");

  ENABLE;
}
