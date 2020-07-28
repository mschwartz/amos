#ifndef KERNEL_X86_CPU_H
#define KERNEL_X86_CPU_H

#include <Types.hpp>
#include <Types/BList.hpp>
#include <Exec/x86/idt.hpp>
#include <Exec/x86/gdt.hpp>

#define LOCALSIZE 0x1000

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

#define CPU_STEPPING_ID(x) (x & 0x0f)
#define CPU_MODEL_ID(x) ((x >> 4) & 0x0f)
#define CPU_FAMILY_ID(x) ((x >> 8) & 0x0f)
#define CPU_PROCESSOR_TYPE(x) ((x >> 12) & 0x03)
#define CPU_EXTENDED_MODEL_ID(x) ((x >> 16) & 0x0f)
#define CPU_EXTENDED_FAMILY_ID(x) ((x >> 20) & 0xff)

class CPU : public BNode {
public:
  CPU(TUint32 aProcessor = 0);

public:
  TUint32 mProcessor; // which core 0-n
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
  IDT *mIdt;
  GDT *mGdt;

public:
  void Dump() {
    dlog("CPU %2d (%x) mMaxFunction(0x%x) mMaxExtendedFunction(0x%x)\n", mProcessor, this, mMaxFunction, mMaxExtendedFunction);
    dlog("  Manufacturer / Model: %s / %s %d cores\n", mManufacturer, mBrand, mCores);
    dlog("    Number of address bits: Physical(%d) Linear(%d)\n", mPhysicalAddressBits, mLinearAddressBits);
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
