#define DEBUGME
#undef DEBUGME

#include <stdint.h>

#include <Exec/ExecBase.hpp>
#include <Inspiration/InspirationBase.hpp>
#include <Graphics/TModeInfo.hpp>

#include <Exec/x86/mmu.hpp>
#include <Exec/x86/pic.hpp>
// #include <Exec/x86/ps2.hpp>
#include <Exec/x86/pci.hpp>
#include <Exec/x86/acpi.hpp>

#include <Devices/AtaDevice.hpp>
#include <Devices/SerialDevice.hpp>
#include <Devices/KeyboardDevice.hpp>
#include <Devices/TimerDevice.hpp>
#include <Devices/RtcDevice.hpp>
#include <Devices/MouseDevice.hpp>

#include <FileSystems/SimpleFileSystem/SimpleFileSystem.hpp>

#include <posix/sprintf.h>
#include <Exec/Random.hpp>
#include <Exec/IdleTask.hpp>
#include <Exec/InitTask.hpp>

ExecBase gExecBase;

extern "C" void schedule_trap();
extern "C" void eputs(const char *s);
extern "C" void enter_tasking();
extern "C" TUint64 rdrand();

// ExecBase constructor
ExecBase::ExecBase() {
  CPU::ColdStart(); // initialize BSP and default Kernel GS

  dlog("ExecBase constructor called\n");
  mDebugSwitch = EFalse;

  SeedRandom64(1);

  dlog("\n\nDisplay Mode table at(0x%x).  Current Mode:\n", gGraphicsModes);
  gGraphicsModes->mDisplayMode.Dump();

  mNumCpus = 0;
  for (TInt i = 0; i < MAX_CPUS; i++) {
    mCpus[i] = ENull;
  }

  // set up paging
  mMMU = new MMU;
  dlog("  initialized MMU\n");

  cli();
  mACPI = new ACPI();
  dlog("  initialized ACPI\n");

  mMessagePortList = new MessagePortList("ExecBase MessagePort List");

  mPci = new PCI();
  dlog("  initialized PCI\n");

  cli();
  InitInterrupts();

  // set up 8259 PIC
  mPIC = new PIC;
  mDisableNestCount = 0;

  //  sti();
  cli();
  dlog("  initialized 8259 PIC\n");

  mCpus[0]->EnterAP();
}

ExecBase::~ExecBase() {
  dlog("ExecBase destructor called\n");
}

void ExecBase::AddCpu(CPU *aCpu) {
  mCpus[mNumCpus++] = aCpu;
}

CPU *ExecBase::CurrentCpu() {
  CPU *cpu = GetCPU();
  // dlog("CurrentCPU(%x)]n", cpu);
  return cpu;
}

TUint64 ExecBase::ProcessorId() {
  CPU *cpu = GetCPU();
  return cpu ? cpu->mProcessorId : 0;
}

void ExecBase::SetInspirationBase(InspirationBase *aInspirationBase) {
  mInspirationBase = aInspirationBase;
  mInspirationBase->Init();
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

void ExecBase::AddTask(BTask *aTask) {
  dlog(">> AddTask(%s)\n", aTask->TaskName());
  AddActiveList(aTask);
}

TInt64 ExecBase::RemoveTask(BTask *aTask, TInt64 aExitCode, TBool aDelete) {
  if (aTask->mCpu) {
    ((CPU *)aTask->mCpu)->RemoveTask(aTask);
  }
  else {
    switch (aTask->mTaskState) {
      case ETaskRunning:
        mActiveTasks.Lock();
        aTask->Remove();
        mActiveTasks.Unlock();
        break;

      case ETaskWaiting:
        mWaitingTasks.Lock();
        aTask->Remove();
        mWaitingTasks.Unlock();
        break;

      default:
        aTask->Remove();
        break;
    }
  }

  if (aDelete) {
    delete aTask;
  }

  return aExitCode;
}

void ExecBase::WaitSemaphore(BTask *aTask, Semaphore *aSemaphore) {
  DISABLE;
  aTask->Remove();
  aTask->mTaskState = ETaskBlocked;
  aSemaphore->mWaitingTasks->AddTail(*aTask);
  aSemaphore->mWaitingCount++;
  aSemaphore->Dump();
  Schedule();
  ENABLE;
}

void ExecBase::ReleaseSemaphore(Semaphore *aSemaphore) {
  DISABLE;
  aSemaphore->mWaitingTasks->Dump();
  BTask *t = aSemaphore->mWaitingTasks->RemHead();
  if (t) {
    aSemaphore->mWaitingCount--;
    aSemaphore->mOwner = t;
    aSemaphore->mNestCount = 1;
    aSemaphore->mSharedCount = 0;
    t->mTaskState = ETaskRunning;
    // CurrentCpu()->AddActiveTask(*t);
    // mActiveTasks.Add(*t);
  }
  else {
    aSemaphore->mOwner = ENull;
    aSemaphore->mNestCount = 0;
    aSemaphore->mSharedCount = 0;
  }
  ENABLE;
}

/**
  * Wake() - if task is on Wait list, move it to active list.  If already on active list, re-add it.
  * Note that Add() will sort the task into the list, effecting round-robin order.
  */
void ExecBase::Wake(BTask *aTask) {
  DISABLE;
  // dlog("-------------- Wake(%s) -------------- \n", aTask->TaskName());
  // note that removing and adding the task will sort the task at the end of all tasks with the same priority.
  // this effects round-robin.
  if (aTask == ENull) {
    mWaitingTasks.Lock();
    if (!mWaitingTasks.Empty()) {
      aTask->Remove();
    }
    mWaitingTasks.Unlock();
  }

  if (aTask == ENull) {
    ENABLE;
    return;
  }

  // TBool test = CompareStrings(aTask->TaskName(), "Init Task") == 0;
  // if (test) {
  //   dlog("-------------- Wake(%s) -------------- \n", aTask->TaskName());
  // }

  switch (aTask->mTaskState) {
    case ETaskRunning:
      mActiveTasks.Lock();
      aTask->Remove();
      mActiveTasks.Unlock();
      break;
    case ETaskWaiting:
      mWaitingTasks.Lock();
      aTask->Remove();
      mWaitingTasks.Unlock();
      break;
    default:
      aTask->Remove();
      break;
  }

  aTask->mTaskState = ETaskRunning;
  AddActiveList(aTask);
  // if (test) {
  //   dlog("WAKE %s\n", aTask->TaskName());
  //   mActiveTasks.Dump();
  //   bochs;
  // }

  ENABLE;
}

void ExecBase::Schedule() {
  DISABLE;
  schedule_trap();
  ENABLE;
}

void ExecBase::Kickstart() {
  enter_tasking(); // just enter next task
}

void ExecBase::AddActiveList(BTask *aTask) {
  mActiveTasks.Lock();
  aTask->mCpu = ENull;
  aTask->mTaskState = ETaskRunning;
  mActiveTasks.Add(*aTask);
  mActiveTasks.Unlock();
}

void ExecBase::AddWaitingList(BTask *aTask) {
  mWaitingTasks.Lock();
  aTask->mCpu = ENull;
  aTask->mTaskState = ETaskWaiting;
  mWaitingTasks.Add(*aTask);
  mWaitingTasks.Unlock();
}

#if 0
BTask *ExecBase::ActivateTask(BTask *aOldTask) {
  // DISABLE;

  if (aOldTask) {
    switch (aOldTask->mTaskState) {
      case ETaskWaiting:
        AddWaitingList(aOldTask);
        break;
      case ETaskRunning:
        AddActiveList(aOldTask);
        break;
      case ETaskBlocked:
        dprint("*** ActivateTask blocked task %s\n", aOldTask->TaskName());
        // Task is on some Semaphore's waiting list
        break;
      default:
        dprint("*** ActivateTask invalid task state(%d) task(%x) %s\n", aOldTask->mTaskState, aOldTask, aOldTask->TaskName());
        bochs
          // shouldn't get here
          break;
    }
    // if (aOldTask->ProcessorId() == 3) {
    //   dlog("-----> Activate %s\n", aOldTask->TaskName());
    // }
  }

  mActiveTasks.Lock();
  BTask *new_task = mActiveTasks.RemHead();
  mActiveTasks.Unlock();

  if (new_task) {
    new_task->mTaskState = ETaskRunning;
  }

  // ENABLE;
  return new_task;
}
#endif

/**
 * Determine next task to run.  This should only be called from IRQ/Interrupt context with interrupts disabled.
 */
void ExecBase::RescheduleIRQ() {
  CPU *c = CurrentCpu();
  c->RescheduleIRQ();
}

BTask *ExecBase::RescheduleTask(BTask *aTask) {
  if (aTask) {
    switch (aTask->mTaskState) {
      case ETaskWaiting:
        // dlog("AddWaitingList(%s)  ", aTask->TaskName());
        AddWaitingList(aTask);
        break;
      case ETaskRunning:
        AddActiveList(aTask);
        break;
      default:
        break;
    }
  }

  mActiveTasks.Lock();
  BTask *new_task = mActiveTasks.RemHead();
  mActiveTasks.Unlock();

  if (new_task) {
    new_task->mTaskState = ETaskRunning;
    // if (aTask != new_task && CompareStrings(new_task->TaskName(), "Init Task") == 0) {
    //   dprint("%s\n", new_task->TaskName());
    // }
  }
  // else {
  //   dprint("NULL Task\n");
  // }

  return new_task;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

TBool ExecBase::AddSemaphore(Semaphore *aSemaphore) {
  mSemaphoreList.Add(*aSemaphore);
  return ETrue;
}

TBool ExecBase::RemoveSemaphore(Semaphore *aSemaphore) {
  mSemaphoreList.Lock();
  if (aSemaphore->mNext && aSemaphore->mPrev) {
    aSemaphore->Remove();
    mSemaphoreList.Unlock();
    return ETrue;
  }
  else {
    mSemaphoreList.Unlock();
    return EFalse;
  }
}

Semaphore *ExecBase::FindSemaphore(const char *aName) {
  mSemaphoreList.Lock();
  Semaphore *s = (Semaphore *)mSemaphoreList.Find(aName);
  mSemaphoreList.Unlock();
  return s;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

void ExecBase::AddMessagePort(MessagePort &aMessagePort) {
  mMessagePortList->Lock();
  mMessagePortList->Add(aMessagePort);
  mMessagePortList->Unlock();
}

TBool ExecBase::RemoveMessagePort(MessagePort &aMessagePort) {
  mMessagePortList->Lock();
  if (mMessagePortList->Find(aMessagePort)) {
    aMessagePort.Remove();
    mMessagePortList->Unlock();
    return ETrue;
  }
  mMessagePortList->Unlock();
  return EFalse;
}

MessagePort *ExecBase::FindMessagePort(const char *aName) {
  mMessagePortList->Lock();
  MessagePort *mp = (MessagePort *)mMessagePortList->Find(aName);
  mMessagePortList->Unlock();
  return mp;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

void ExecBase::AddDevice(BDevice *aDevice) {
  mDeviceList.Lock();
  mDeviceList.Add(*aDevice);
  mDeviceList.Unlock();
  ;
}

BDevice *ExecBase::FindDevice(const char *aName) {
  mDeviceList.Lock();
  BDevice *d = mDeviceList.FindDevice(aName);
  mDeviceList.Unlock();
  return d;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

void ExecBase::AddFileSystem(BFileSystem *aFileSystem) {
  mFileSystemList.Lock();
  mFileSystemList.AddHead(*aFileSystem);
  mFileSystemList.Unlock();
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

void ExecBase::GuruMeditation(const char *aFormat, ...) {
  cli();
  CPU *c = GetCPU();
  va_list args;
  va_start(args, aFormat);
  c->GuruMeditation(aFormat, args);
  // bochs;
  // char buf[512];
  // dprint("\n\n***********************\n");
  // dprint("GURU MEDITATION at %dms\n", SystemTicks());

  // va_list args;
  // va_start(args, aFormat);
  // vsprintf(buf, aFormat, args);
  // dprint(buf);
  // dprint("\n");

  // GetCurrentTask()->Dump();
  // va_end(args);
  // dprint("***********************\n\n\nHalted.\n");

  for (;;) {
    halt();
  }
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

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
  // dlog("SetIntVector(%d, %x) %s\n", aInterruptNumber, aInterrupt, aInterrupt->mNodeName);
  mInterrupts[aInterruptNumber].Add(*aInterrupt);
}

void ExecBase::EnableIRQ(TUint16 aIRQ) {
  IDT::EnableInterrupt(aIRQ);
  mACPI->EnableIRQ(aIRQ);
}

void ExecBase::DisableIRQ(TUint16 aIRQ) {
  IDT::DisableInterrupt(aIRQ);
  mACPI->DisableIRQ(aIRQ);
}

void ExecBase::AckIRQ(TUint16 aIRQ) {
  mACPI->AckIRQ(aIRQ);
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
      list->Unlock();
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
  IDT::EnableInterrupt(aIndex);
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
  IDT::EnableInterrupt(aIndex);
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
