#ifndef EXEC_EXECBASE_H
#define EXEC_EXECBASE_H

#include <Exec/BBase.hpp>
#include <Exec/BInterrupt.hpp>
#include <Exec/MessagePort.hpp>
#include <Exec/BDevice.hpp>
#include <Exec/BFileSystem.hpp>
#include <Exec/BTask.hpp>
#include <Exec/BProcess.hpp>
#include <Exec/Random.hpp>
#include <Exec/CPU.hpp>
#include <Exec/Semaphore.hpp>
#include <Exec/TSysInfo.hpp>

extern "C" TUint64 GetFlags();
extern "C" void SetFlags(TUint64 aFlags);

#define DISABLE                  \
  TUint64 ___flags = GetFlags(); \
  cli();
#define ENABLE SetFlags(___flags);

//#include <Devices/Screen.h>
//#include <Exec/x86/cpu.h>

class RtcDevice;
// class TSS;
// class GDT;
class MMU;
// class IDT;
class PIC;
class PS2;
class ACPI;
class PCI;
class CPU;
class InspirationBase;

class IdleTask;
class InitTask;

class ExecBase : public BBase {
  friend RtcDevice;
  friend IdleTask;
  friend InitTask;
  friend CPU;
  friend Semaphore; // Semaphore needs to manipulate current task and task list information

protected:
  TUint64 Tick() {
    gSystemInfo.mMillis++;
    // randomize RNG as long as we're here
    Random();
    return gSystemInfo.mMillis;
  }

public:
  ExecBase();
  ~ExecBase();

  /**
    * Return quantum (timer interrupt frequency for multitasking preemption) in hz
    */
  TInt Quantum() { return 100; }

public:
  void GuruMeditation(const char *aFormat, ...);

public:
  InspirationBase *GetInspirationBase() { return mInspirationBase; }
  void SetInspirationBase(InspirationBase *aInspirationBase);

protected:
  InspirationBase *mInspirationBase;

protected:
  MMU *mMMU;
  PIC *mPIC;
  ACPI *mACPI;

public:
  // TPciDevice *FirstPciDevice() { return mPci->First(); }
  // TPciDevice *NextPciDevice(TPciDevice *aDevice) { return mPci->Next(aDevice); }
  // TPciDevice *EndPciDevices(TPciDevice *aDevice) { return mPci->End(aDevice); }

  PCI *GetPci() { return mPci; }

protected:
  PCI *mPci;

public:
  TInt NumCpus() { return mNumCpus; }
  void AddCpu(CPU *aCPU);
  CPU *CurrentCpu();
  CPU *GetCpu(TInt aNum) { return mCpus[aNum]; }
  void InterruptOthers(TUint8 aVector);
  TUint64 GetCurrentCpuNumber();

protected:
  TInt mNumCpus;
  CPU *mCpus[MAX_CPUS];

public:
  PS2 *GetPS2() { return mPS2; }

protected:
  PS2 *mPS2;

  //
  // INTERRUPTS
  //
public:
  void SetIntVector(EInterruptNumber aInterruptNumber, BInterrupt *aInterrupt);
  void RemoveIntVector(BInterrupt *aInterrupt);

  void EnableIRQ(TUint16 aIRQ);
  void DisableIRQ(TUint16 aIRQ);
  void AckIRQ(TUint16 aIRQ);

  //
protected:
  // interrupt lists
  static TBool RootHandler(TInt64 aInterruptNumber, TAny *aData);
  void SetInterrupt(EInterruptNumber aInterruptNumber, const char *aName);
  void SetException(EInterruptNumber aInterruptNumber, const char *aName);
  void SetTrap(EInterruptNumber aInterruptNumber, const char *aName);
  void InitInterrupts();
  //
  BInterruptList mInterrupts[EMaxInterrupts];

public:
  void Enable();
  void Disable();

protected:
  volatile TInt64 mDisableNestCount;

  //
  // TASKS
  //
public:
  // add task to active task list
  void AddTask(BTask *aTask);
  // suicide/exit/kill task
  TInt64 RemoveTask(BTask *aTask, TInt64 aExitCode, TBool aDelete = ETrue);

  // void DumpTasks();
  // void DumpCurrentTask() { mCurrentTask->Dump(); }
  void AddWaitingList(BTask &aTask);

  BTask *GetCurrentTask() {
    CPU *cpu = CurrentCpu();
    if (cpu) {
      return cpu->CurrentTask();
    }
    return ENull;
  }

  const char *CurrentTaskName() {
    BTask *t = GetCurrentTask();
    if (t) {
      return t->mNodeName;
    }
    else {
      return "NO TASK";
    }
  }

  /**
    * Put task to sleep until any of its sigwait signals are set.
    */
  void WaitSignal(BTask *aTask);
  //  void Wait(BTask *aTask);
  void Wake(BTask *aTask = ENull);

  void Kickstart();     // kickstart multitasking.  Only call once from main() !!!!
                        //  void Reschedule();
  void RescheduleIRQ(); // from IRQ context
  void Schedule();

protected:
  BTaskList mWaitingTasks;

  //
  // Semaphores
  //
public:
  TBool AddSemaphore(Semaphore *aSemaphore);
  TBool RemoveSemaphore(Semaphore *aSemaphore);
  Semaphore *FindSemaphore(const char *aName);
  void WaitSemaphore(BTask *aTask, Semaphore *aSemaphore);
  void ReleaseSemaphore(Semaphore *aSemaphore);

protected:
  SemaphoreList mSemaphoreList;

  //
  // Message Ports
  //
public:
  void AddMessagePort(MessagePort &aMessagePort);
  TBool RemoveMessagePort(MessagePort &aMessagePort);

  MessagePort *FindMessagePort(const char *aPortName);

protected:
  MessagePortList *mMessagePortList;

  //
  // DEVICES
  //
public:
  void AddDevice(BDevice *aDevice);
  BDevice *FindDevice(const char *aName);

protected:
  BDeviceList mDeviceList;

  //
  // FILESYSTEMS
  //
public:
  void AddFileSystem(BFileSystem *aFileSystem);
  BFileSystem *FindFileSystem(const char *aName);
  BFileSystem *FirstFileSystem() { return (BFileSystem *)mFileSystemList.First(); }
  BFileSystem *NextFileSystem(BFileSystem *aFileSystem) { return (BFileSystem *)mFileSystemList.Next(aFileSystem); }
  TBool EndFileSystems(BFileSystem *aFileSystem) { return mFileSystemList.End(aFileSystem); }

protected:
  BFileSystemList mFileSystemList;

public:
  TUint64 SystemTicks() { return gSystemInfo.mMillis; }

public:
  void GetSystemInfo(TSystemInfo *aSystemInfo) {
    *aSystemInfo = gSystemInfo;
  }

public:
  TBool mDebugSwitch;
};

extern ExecBase gExecBase;

#endif
