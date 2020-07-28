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

extern "C" TUint64 GetFlags();
extern "C" void SetFlags(TUint64 aFlags);

#define DISABLE                  \
  TUint64 ___flags = GetFlags(); \
  cli();
#define ENABLE SetFlags(___flags);

//#include <Devices/Screen.h>
//#include <Exec/x86/cpu.h>

class RtcDevice;
class TSS;
class GDT;
class MMU;
class IDT;
class PIC;
class PS2;
class ACPI;
class PCI;
class CPU;
class CPUList;
class InspirationBase;

class IdleTask;
class InitTask;

/* External interrupts */
#define IRQ_OFFSET 0x20 /* Interrupt offset for external interrupts */
enum {
  IRQ_TIMER = IRQ_OFFSET, // 0
  IRQ_KEYBOARD,           // 1
  IRQ_SLAVE_PIC,          // 2
  IRQ_COM2,               // 3
  IRQ_COM1,               // 4
  IRQ_LPT2,               // 5
  IRQ_FLOPPY,             // 6
  IRQ_LPT1,               // 7
  IRQ_RTC,                // 8
  IRQ_MASTER_PIC,         // 9
  IRQ_RESERVED1,          // 10
  IRQ_RESERVED2,          // 11
  IRQ_MOUSE,              // 12
  IRQ_COPROCESSOR,        // 13
  IRQ_ATA1,               // 14
  IRQ_ATA2,               // 15

  HW_INTERRUPTS
};

typedef struct {
  TUint64 mMillis;
  TUint64 mEBDA; // Extended BIOS Data Area
  TUint64 mRam;

  TUint64 mVideoMode;
  TAny *mDisplayFrameBuffer;
  TInt64 mDisplayWidth;
  TInt64 mDisplayHeight;
  TInt64 mDisplayDepth;
  TInt64 mDisplayPitch;

  TUint64 mBootSector;
  TUint64 mBootSectors;
  TUint64 mKernelSector;
  TUint64 mKernelSectors;
  TUint64 mRootSector;

  TUint64 mBootDrive;
  TUint64 mNumDrives;
  TUint64 mNumHeads;
  TUint64 mNumCylinders;
  TUint64 mNumSectors;
  TUint64 mDiskCX;

  TUint64 mCpuMhz;
  TUint64 mCpuCores;
  TUint64 mBochs;
  TUint64 mInitStart, mInitEnd;
  TUint64 mTextStart, mTextEnd;
  TUint64 mRoDataStart, mRoDataEnd;
  TUint64 mDataStart, mDataEnd;
  TUint64 mBssStart, mBssEnd;
  TUint64 mKernelEnd;
  TUint64 mDiskSize;
  char mVersion[64];

  void Dump() {
    // dlog("TSystemInfo(%x)\n", this);
    dlog("%s %s\n", mVersion, mBochs ? "BOCHS ENABLED" : "NO BOCHS");
    dlog("     CPU Speed: %0d Mhz\n", mCpuMhz);
    dlog("          init: %016x - %016x\n", mInitStart, mInitEnd);
    dlog("          text: %016x - %016x\n", mTextStart, mTextEnd);
    dlog("        rodata: %016x - %016x\n", mRoDataStart, mRoDataEnd);
    dlog("          data: %016x - %016x\n", mDataStart, mDataEnd);
    dlog("           bss: %016x - %016x\n", mBssStart, mBssEnd);
    dlog("    kernel_end: %016x\n", mKernelEnd);
    dlog("          ebda: %016x\n", mEBDA);
    dlog("\n");
    dlog("  mDisplayWidth: %d\n", mDisplayWidth);
    dlog(" mDisplayHeight: %d\n", mDisplayHeight);
    dlog("  mDisplayDepth: %d\n", mDisplayDepth);
    dlog("  mDisplayPitch: %d\n", mDisplayPitch);
    dlog("  mFrameBuffer: %x\n", mDisplayFrameBuffer);
    dlog("\n");
    dlog("    mNumDrives: %d\n", mNumDrives);
    dlog("    mBootDrive: %02x\n", mBootDrive);
    dlog("       mDiskCX: %02x\n", mDiskCX);
    dlog("     mNumHeads: %d\n", mNumHeads);
    dlog(" mNumCylinders: %d\n", mNumCylinders);
    dlog("   mNumSectors: %d\n", mNumSectors);
    dlog("     mDiskSize: %d\n", mDiskSize);
    dlog("\n");
    dlog("   mBootSector: %d\n", mBootSector);
    dlog("  mBootSectors: %d\n", mBootSectors);
    dlog(" mKernelSector: %d\n", mKernelSector);
    dlog("mKernelSectors: %d\n", mKernelSectors);
    dlog("   mRootSector: %d\n", mRootSector);

    dprint("\n");
  }
} PACKED TSystemInfo;

static TSystemInfo &gSystemInfo = *(TSystemInfo *)0x5000; // see memory.inc

class ExecBase : public BBase {
  friend RtcDevice;
  friend IdleTask;
  friend InitTask;

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
  TSS *mTSS;
  GDT *mGDT;
  MMU *mMMU;
  IDT *mIDT;
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
  void AddCpu(CPU *aCPU);

protected:
  CPUList *mCpuList;
  
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

  void DumpTasks();
  void DumpCurrentTask() { mCurrentTask->Dump(); }
  BTask *GetCurrentTask() { return mCurrentTask; }
  const char *CurrentTaskName() { return mCurrentTask ? mCurrentTask->mNodeName : "NO TASK"; }
  /**
    * Put task to sleep until any of its sigwait signals are set.
    */
  void WaitSignal(BTask *aTask);
  //  void Wait(BTask *aTask);
  void Wake(BTask *aTask);

  void Kickstart();     // kickstart multitasking.  Only call once from main() !!!!
                        //  void Reschedule();
  void RescheduleIRQ(); // from IRQ context
  void Schedule();

protected:
  BTaskList mActiveTasks, mWaitingTasks;
  BTask *mCurrentTask;

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

  // protected:
  //   TSystemInfo mSystemInfo;

public:
  void GetSystemInfo(TSystemInfo *aSystemInfo) {
    *aSystemInfo = gSystemInfo;
  }
};

extern ExecBase gExecBase;

#endif