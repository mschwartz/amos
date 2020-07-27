#ifndef KERNEL_X86_CPU_H
#define KERNEL_X86_CPU_H

#include <Types.h>
#include <Types/BList.h>

#define LOCALSIZE 0x1000

class CPU  : public BNode {
public:
  CPU();
public:
  char mManufacturer[16];
};

class CPUList : public BList {
public:
  CPUList() : BList("CPUList") {}
};

#if 0
// https://en.wikipedia.org/wiki/CPUID
typedef struct {
  TUint32 mProcessorInfo;
  TUint8 mSteppingId;
  TUint8 mModel;
  TUint8 mFamilyId;
  TUint8 mProcessorType;
  TUint8 mExtendedModelId;
  TUint8 mExtendedFamilyId;
  //
  TUint32 mAdditionalInformation;
  TUint32 mFeatureInformation1;
  TUint32 mFeatureInformation2;
  //
  char mProcessorBrandString[64];

public:
  void SetProcessorInfo(TUint32 aFeatures) {
    mFeatures = aFeatures;
    mSteppingId = mFeatures & 0x0f;
    mModel = (mFeatures >> 4) & 0x0f;
    mFamilyId = (mFeatures >> 8) & 0x0f;
    mProcessorType = (mFeatures >> 12) & 0x03;
    mExtendedModelId = (mFeatures >> 16) & 0x0f;
    mExtendedFamilyId = (mFeatures >> 20) & 0xff;
  }

public:
  void Dump() {
    dlog("TCpuInfo(%x)\n", this);
    dlog("  mManufacturer(%s)\n", mManufacturer);
    dlog("  mFeatures: %08x\n", mFeatures);
    dlog("    mModel: %02x\n", mModel);
    dlog("    mFamilyId: %02x\n", mFamilyId);
    dlog("    mmProcessorType: %02x\n", mmProcessorType);
    dlog("    mExtendedModelId: %02x\n", mExtendedModelId);
    dlog("    mExtendedFamilyId: %02x\n", mExtendedFamilyId);
  }
} PACKED TCpuInfo;
#endif

#if 0
struct CPU {
  TUint8 mLocal[LOCALSIZE] ALIGN16;
  TInt16 mDisableDepth;
  TUint64 mTicks;
  TUint16 mPreemptionDepth;
  TUint32 mSpeed;
  // BThread *mCurrentThread;
public:
  CPU() {
    for (TInt i=0; i<LOCALSIZE; i++) {
      mLocal[i] = 0;
    }
    mDisableDepth = 0;
    mTicks = 0;
    mPreemptionDepth = 0;
    mSpeed = 0;
  }
} PACKED;
#endif

#endif
