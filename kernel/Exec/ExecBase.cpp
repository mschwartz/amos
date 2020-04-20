#include <Exec/ExecBase.h>
#include <x86/bochs.h>
#include <stdint.h>

#include <Devices/KeyboardDevice.h>
#include <Devices/TimerDevice.h>

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
    dprint("IdleTask Running\n");
    while (1) {
      halt();
    }
  }
};

ExecBase::ExecBase() {
  in_bochs = *((TUint8 *)0x7c10);
  dprint("bochs %x\n", in_bochs);

  dprint("ExecBase constructor called\n");

  mMessagePortList = new BMessagePortList("ExecBase MessagePort List");

  //  Screen s;
  mScreen = new Screen;
  dprint("  initialized screen\n");

  mGDT = new GDT;
  dprint("  initialized GDT\n");

  // set up paging
  mMMU = new MMU;
  dprint("  initialized MMU\n");

  mIDT = new IDT;
  dprint("  initialized IDT\n");

  InitInterrupts();
  //  dprint("  Installed interrupt handlers\n");

  //  mCPU = new CPU;
  //  dprint("  initialized CPU vectors\n");

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
  dprint("  initialized 8259 PIC\n");
  Enable();


  // initialize devices
  AddDevice(mTimer = new TimerDevice());
  kprint("  initialized timer\n");

  AddDevice(new KeyboardDevice);
  kprint("  initialized keyboard \n");
}

ExecBase::~ExecBase() {
  dprint("ExecBase destructor called\n");
}

TUint64 ExecBase::SystemTicks() {
  return mTimer->GetTicks();
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
  dprint("Add Task %x rip=%x rsp=%x --- ", aTask, aTask->mRegisters.rip, aTask->mRegisters.rsp);
  dputs(aTask->mNodeName);
  dprint(" ---\n");
  Disable();
  mActiveTasks.Add(*aTask);
  Enable();
//  Reschedule();
//  Schedule();
}

void ExecBase::WaitSignal(BTask *aTask) {
  aTask->Remove();
  // If task has received a signal it's waiting for, we don't want to move it to the WAIT list,
  // but it may be lower priority than another task so we need to sort it in to ACTIVE list.
  if (aTask->mSigWait & aTask->mSigReceived) {
//    dputs("wait signal active ");
//    dputs(aTask->mNodeName);
    dputs("\n");
    mActiveTasks.Add(*aTask);
    aTask->mTaskState = ETaskRunning;
  }
  else {
//    dputs("wait signal wait ");
//    dputs(aTask->mNodeName);
//    dputs("\n");
    mWaitingTasks.Add(*aTask);
    aTask->mTaskState = ETaskWaiting;
//    mCurrentTask = mActiveTasks.First();
  }
//  Reschedule();
  Schedule();
}

#if 0
// assumes aTask is in Active list
void ExecBase::Wait(BTask *aTask) {
  Disable();
  // note that removing and adding the task will sort the task at the end of all tasks with the same priority.
  // this effects round-robin.
  aTask->Remove();
  if (aTask->mSigWait & aTask->mSigReceived) {
    mActiveTasks.Add(*aTask);
  }
  else {
    mWaitingTasks.Add(*aTask);
  }
  Enable();
  Reschedule();
  Schedule();
}
#endif

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

/**
  * Determine next task to run. Should be called when logic determines that a different task, than the currently
  * running one, could be made active.
  */
void ExecBase::Reschedule() {
  Disable();
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
  next_task = &mCurrentTask->mRegisters;
  Enable();
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

void ExecBase::GuruMeditation(const char *aMessage) {
  cli();
  dputs("\n\n***********************\n");
  dputs("GURU MEDIDTATION\n");
  if (aMessage) {
    dprint(aMessage);
    dprint("\n");
  }

  mCurrentTask->Dump();
  dputs("***********************\n\n\n");
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
    cli();
    //    dprintf(mNodeName);
    //    dprintf(" Exception\n");
    gExecBase.GuruMeditation(mNodeName);
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
    dprintf(mNodeName);
    dprintf(" IRQ\n");
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
    //    dprintf(mNodeName);
    //    dprintf(" TRAP\n");
    return ETrue;
  }
};

void ExecBase::SetIntVector(EInterruptNumber aInterruptNumber, BInterrupt *aInterrupt) {
  //  dprint("SetIntVector(%d) ", aInterruptNumber);
  //  dprint(aInterrupt->mNodeName);
  //  dprint("\n");

  mInterrupts[aInterruptNumber].Add(*aInterrupt);
}

TBool ExecBase::RootHandler(TInt64 aInterruptNumber, TAny *aData) {
  //  dprint("RootHandler! %d %x\n", aInterruptNumber, aData);
  BInterruptList *list = &gExecBase.mInterrupts[aInterruptNumber];
  for (BInterrupt *i = (BInterrupt *)list->First(); !list->End(i); i = (BInterrupt *)i->mNext) {
    //    dprint("calling %d\n", i->pri);
    if (i->Run(aData)) {
      return ETrue;
    }
  }
  // TODO: no handler!
  dprint("No handler!\n");
  return EFalse;
}

void ExecBase::SetException(EInterruptNumber aIndex, const char *aName) {
  IDT::install_handler(aIndex, ExecBase::RootHandler, this, aName);
  SetIntVector(aIndex, new DefaultException(aName));
}

void ExecBase::SetInterrupt(EInterruptNumber aIndex, const char *aName) {
  //  dprint("Add IRQ %d ", aIndex);
  //  dprint(aName);
  //  dprint("\n");
  IDT::install_handler(aIndex, ExecBase::RootHandler, this, aName);
  SetIntVector(aIndex, new DefaultIRQ(aName));
}

void ExecBase::SetTrap(EInterruptNumber aIndex, const char *aName) {
  dprint("Add Trap %d ", aIndex);
  dprint(aName);
  dprint("\n");
  IDT::install_handler(aIndex, ExecBase::RootHandler, this, aName);
  SetIntVector(aIndex, new NextTaskTrap(aName));
}

void ExecBase::InitInterrupts() {
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
  SetInterrupt(ERtClockIRQ, "RtClock");
  SetInterrupt(EMasterPicIRQ, "MasterPic");
  SetInterrupt(EReserved1IRQ, "Reserved1");
  SetInterrupt(EReserved2IRQ, "Reserved2");
  SetInterrupt(EReserved3IRQ, "Reserved3");
  SetInterrupt(ECoprocessorIRQ, "Coprocessor");
  SetInterrupt(EHardDiskIRQ, "HardDisk");
  //  SetInterrupt(EReserved4IRQ, "Reserved4");
  SetTrap(ETrap0, "Trap0");
}
