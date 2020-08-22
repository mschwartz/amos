#define DEBUGME
#undef DEBUGME

#include <stdint.h>

#include <Exec/ExecBase.hpp>
#include <Inspiration/InspirationBase.hpp>
#include <Graphics/TModeInfo.hpp>

#include <Exec/x86/mmu.hpp>
#include <Exec/x86/pic.hpp>
#include <Exec/x86/ps2.hpp>
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
  CPU::ColdStart();
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

#ifdef ENABLE_PS2
  mPS2 = new PS2();
#else
  mPS2 = ENull;
#endif

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

TUint64 ExecBase::GetCurrentCpuNumber() {
  CPU *cpu = GetCPU();
  return cpu ? cpu->mProcessorId : 0;
}

void ExecBase::SetInspirationBase(InspirationBase *aInspirationBase) {
  mInspirationBase = aInspirationBase;
  mInspirationBase->Init();
}

void ExecBase::InterruptOthers(TUint8 aVector) {
  CPU *cpu = mCpus[0];
  if (cpu != ENull) {
    cpu->mApic->InterruptOthers(aVector);
  }
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

static Mutex tasks_mutex;

void ExecBase::AddTask(BTask *aTask) {
  DISABLE;
  tasks_mutex.Acquire();
  mRunningTasks.Add(*aTask);
  tasks_mutex.Release();
  ENABLE;
}

TInt64 ExecBase::RemoveTask(BTask *aTask, TInt64 aExitCode, TBool aDelete) {

  DISABLE;
  CPU *c = CurrentCpu();
  if (!c) {
    c = CurrentCpu();
    bochs;
  }
  c->RemoveTask(aTask, aExitCode);
  ENABLE;

  if (aDelete) {
    delete aTask;
  }

  return aExitCode;
}

void ExecBase::WaitSignal(BTask *aTask) {
  DISABLE;
  //
  tasks_mutex.Acquire();
  aTask->Remove();
  tasks_mutex.Release();

  // If task has received a signal it's waiting for, we don't want to move it to the WAIT list,
  // but it may be lower priority than another task so we need to sort it in to ACTIVE list.
  if (aTask->mSigWait & aTask->mSigReceived) {
    aTask->mTaskState = ETaskRunning;
    tasks_mutex.Acquire();
    mRunningTasks.Add(*aTask);
    tasks_mutex.Release();
  }
  else {
    aTask->mTaskState = ETaskWaiting;
    tasks_mutex.Acquire();
    mWaitingTasks.Add(*aTask);
    tasks_mutex.Release();
  }
  Schedule();
  ENABLE;
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
    CurrentCpu()->AddActiveTask(*t);
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
  // note that removing and adding the task will sort the task at the end of all tasks with the same priority.
  // this effects round-robin.
  DISABLE;
  if (aTask) {
    tasks_mutex.Acquire();
    if (aTask->mNext) {
      aTask->Remove();
    }
    aTask->mTaskState = ETaskRunning;
    mRunningTasks.Add(*aTask);
    tasks_mutex.Release();
  }
  // CurrentCpu()->AddActiveTask(*aTask);
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
  CPU *c = CurrentCpu();
  c->RescheduleIRQ();
}

BTask *ExecBase::NextTask(BTask *aTask) {
  DISABLE;
  tasks_mutex.Acquire();

  if (aTask != ENull) {
    switch (aTask->mTaskState) {
      case ETaskRunning:
        mRunningTasks.Add(*aTask);
        break;
      case ETaskWaiting:
        mWaitingTasks.Add(*aTask);
        break;
    }
  }

  BTask *ret = mRunningTasks.RemHead();

  tasks_mutex.Release();
  ENABLE;
  return ret;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

static Mutex sem_mutex;

TBool ExecBase::AddSemaphore(Semaphore *aSemaphore) {
  sem_mutex.Acquire();
  mSemaphoreList.Add(*aSemaphore);
  sem_mutex.Release();
  return ETrue;
}

TBool ExecBase::RemoveSemaphore(Semaphore *aSemaphore) {
  sem_mutex.Acquire();
  if (aSemaphore->mNext && aSemaphore->mPrev) {
    aSemaphore->Remove();
    sem_mutex.Release();
    return ETrue;
  }
  sem_mutex.Release();
  return EFalse;
}

Semaphore *ExecBase::FindSemaphore(const char *aName) {
  DISABLE;
  sem_mutex.Acquire();
  Semaphore *s = (Semaphore *)mSemaphoreList.Find(aName);
  sem_mutex.Release();
  ENABLE;
  return s;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

void ExecBase::AddMessagePort(MessagePort &aMessagePort) {
  DISABLE;
  mMessagePortList->Lock();
  mMessagePortList->Add(aMessagePort);
  mMessagePortList->Unlock();
  ENABLE;
}

TBool ExecBase::RemoveMessagePort(MessagePort &aMessagePort) {
  if (mMessagePortList->Find(aMessagePort)) {
    DISABLE;
    mMessagePortList->Lock();
    aMessagePort.Remove();
    mMessagePortList->Unlock();
    ENABLE;
    return ETrue;
  }
  return EFalse;
}

MessagePort *ExecBase::FindMessagePort(const char *aName) {
  DISABLE;
  mMessagePortList->Lock();
  MessagePort *mp = (MessagePort *)mMessagePortList->Find(aName);
  mMessagePortList->Unlock();
  ENABLE;
  return mp;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

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

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

void ExecBase::AddFileSystem(BFileSystem *aFileSystem) {
  DISABLE;
  mFileSystemList.AddHead(*aFileSystem);
  ENABLE;
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
  dlog("SetIntVector(%d, %x) %s\n", aInterruptNumber, aInterrupt, aInterrupt->mNodeName);
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
