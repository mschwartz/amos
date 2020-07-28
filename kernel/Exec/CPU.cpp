#include <Exec/CPU.hpp>
#include <Exec/ExecBase.hpp>

extern "C" TUint64 cpuid(TUint32 *eax, TUint32 *ebx, TUint32 *ecx, TUint32 *edx);
extern "C" void cpu_brand(char *buf);

CPU::CPU(TUint32 aProcessor) : BNode("CPU") {
  mProcessor = aProcessor;
  TUint32 eax, ebx, edx, ecx;

  mIdt = gExecBase.mIDT;
  mGdt = gExecBase.mGDT;

  eax = 0;
  cpuid(&eax, &ebx, &ecx, &edx);
  CopyMemory(mManufacturer, (char *)&ebx, 12);
  mManufacturer[12] = '\0';

  mMaxFunction = eax;

  eax = 1;
  cpuid(&eax, &ebx, &ecx, &edx);
  mProcessorVersionInformation = eax;
  mProcessorAdditionalInformation = ebx;
  mProcessorFeatureInformation1 = edx; // bit 0 is FPU
  mProcessorFeatureInformation2 = ecx; // bit 0 is SSE3

  eax = 7;
  cpuid(&eax, &ebx, &ecx, &edx);
  mFeatures1 = ebx;
  mFeatures2 = ecx;
  mFeatures3 = edx;

  eax = 0x80000000;
  cpuid(&eax, &ebx, &ecx, &edx);
  mMaxExtendedFunction = eax;

  eax = 0x80000001;
  cpuid(&eax, &ebx, &ecx, &edx);
  mExtendedFeatures1 = edx;
  mExtendedFeatures2 = ecx;

  cpu_brand(&mBrand[0]);

  if (mMaxExtendedFunction <= 0x80000008) {
    eax = 0x80000008;
    cpuid(&eax, &ebx, &ecx, &edx);
    mPhysicalAddressBits = eax & 0xff;
    mLinearAddressBits = (eax >> 8) & 0xff;
    mCores = (ecx & 0xff) + 1;
    mApicIdBits = (ecx >> 12) & 0x0f;
  }
  else {
    mPhysicalAddressBits = 0;
    mLinearAddressBits = 0;
    mCores = 0;
    mApicIdBits = 0;
  }

  Dump();
}
