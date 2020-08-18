#ifndef KERNEL_X86_CPU_H
#define KERNEL_X86_CPU_H

// https://ethv.net/workshops/osdev/notes-notes-3

// TODO GS register points to this CPU's info area

#include <Types.hpp>
#include <Exec/x86/acpi.hpp>
#include <Exec/x86/ioapic.hpp>
#include <Exec/x86/apic.hpp>
#include <Exec/x86/idt.hpp>
#include <Exec/x86/gdt.hpp>
#include <Exec/x86/tss.hpp>
#include <Exec/BTask.hpp>
#include <Exec/Mutex.hpp>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

const TInt BOOT_STACK_SIZE = 16384;

enum {
  ECpuUninitialized,
  ECpuUnusable,
  ECpuInitialized,
  ECpuRunning,
  ECpuHalted,
  ECpuGuruMeditation,
} ECpuState;

#define CPU_STEPPING_ID(x) (x & 0x0f)
#define CPU_MODEL_ID(x) ((x >> 4) & 0x0f)
#define CPU_FAMILY_ID(x) ((x >> 8) & 0x0f)
#define CPU_PROCESSOR_TYPE(x) ((x >> 12) & 0x03)
#define CPU_EXTENDED_MODEL_ID(x) ((x >> 16) & 0x0f)
#define CPU_EXTENDED_FAMILY_ID(x) ((x >> 20) & 0xff)

class ACPI;
class ExecBase;
class IdleTask;
class TimerInterrupt;

class CPU : public BBase {
  friend ExecBase;
  friend TimerInterrupt;

public:
  // constructor
  CPU(TUint32 aProcessorId, TUint32 aApicId, ACPI *aAcpi);

public:
  static void ColdStart();

public:
  // Initialize must be called from the actual running CPU (e.g. not the BSP)
  // void Initialize();
  void StartAP(BTask *aTask); // perform SIPI to start AP
  void EnterAP();             // entry point for AP, running in the AP's CORE!

public:
  void AckIRQ(TUint16 aIRQ);

protected:
  GDT *mGdt;
  TSS *mTss;
  IDT *mIdt;
  TGS mGS;
  IdleTask *mIdleTask;

public:
  void GuruMeditation(const char *aFormat, ...);

protected:
  void AddTask(BTask *aTask);
  TInt64 RemoveTask(BTask *aTask, TInt64 aExitCode);
  BTask *CurrentTask() { return mCurrentTask; }
  void DumpTasks();

  void AddActiveTask(BTask &aTask) {
    mMutex.Acquire();
    mActiveTasks.Add(aTask);
    mMutex.Release();
  }

  void WaitSignal(BTask *aTask);
  void RescheduleIRQ();

protected:
  Mutex mMutex;
  TInt64 mRunningTaskCount;;
  BTaskList mActiveTasks;
  BTask *mCurrentTask;

public:
  TUint8 *mBootStack;
  TUint64 mCpuState;

public:
  TUint32 mProcessorId;
  TUint32 mApicId;
  // IoApic *mIoApic;
  ACPI *mAcpi;
  Apic *mApic; // local APIC

public:
  // Data gathered via CPUID
  TUint32 mMaxFunction;
  TUint32 mProcessorVersionInformation;
  TUint32 mProcessorAdditionalInformation;
  TUint32 mProcessorFeatureInformation1;
  TUint32 mProcessorFeatureInformation2;
  TUint32 mFeatures1;
  TUint32 mFeatures2;
  TUint32 mFeatures3;
  //
  TUint32 mMaxExtendedFunction;
  TUint32 mExtendedFeatures1;
  TUint32 mExtendedFeatures2;
  //
  TUint32 mPhysicalAddressBits;
  TUint32 mLinearAddressBits;
  TUint32 mCores; // number of CPU cores
  TUint32 mApicIdBits;
  //
  char mManufacturer[16];
  char mBrand[64];
  //

public:
  void Dump() {
    dprint("\n\n");
    dlog("CPU mProcessorId(%d) mApicId(%d) mAcpi(0x%x) this(%x)\n",
      mProcessorId, mApicId, mAcpi, this);
    dlog("  Manufacturer / Model: %s / %s %d cores\n", mManufacturer, mBrand, mCores);
    dlog("    Number of address bits: Physical(%d) Linear(%d)\n",
      mPhysicalAddressBits, mLinearAddressBits);
    dlog("    mMaxFunction: (0x%x) mMaxExtendedFunction(0x%x)\n", mMaxFunction, mMaxExtendedFunction);
    dlog("    CPU Stepping ID: %x\n", CPU_STEPPING_ID(mProcessorVersionInformation));
    dlog("    CPU Model ID: %x\n", CPU_MODEL_ID(mProcessorVersionInformation));
    dlog("    CPU Family ID: %x\n", CPU_FAMILY_ID(mProcessorVersionInformation));
    dlog("    CPU Processor Type: %x\n", CPU_PROCESSOR_TYPE(mProcessorVersionInformation));
    dlog("    CPU Extended Model ID: %x\n", CPU_EXTENDED_MODEL_ID(mProcessorVersionInformation));
    dlog("    CPU Extended Family ID: %x\n", CPU_EXTENDED_FAMILY_ID(mProcessorVersionInformation));
    // dlog("    mProcessorVersionInformation: %08x\n", mProcessorVersionInformation);
    dlog("    mProcessorAdditionalInformation: %08x\n", mProcessorAdditionalInformation);
    dlog("    mProcessorFeatureInformation1: %08x\n", mProcessorFeatureInformation1);
    dlog("    mProcessorFeatureInformation2: %08x\n", mProcessorFeatureInformation1);
    dlog("    mFeatures1: %08x\n", mFeatures1);
    dlog("    mFeatures2: %08x\n", mFeatures2);
    dlog("    mFeatures3: %08x\n", mFeatures3);
    dlog("    mExtendedFeatures1: %08x\n", mExtendedFeatures1);
    dlog("    mExtendedFeatures2: %08x\n", mExtendedFeatures2);
  }
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class CPUList : public BList {
public:
  CPUList() : BList("CPUList") {}
};

#endif
