#include "cpu.h"

extern "C" TUint64 cpuid(TUint32 *eax, TUint32 *ebx, TUint32 *ecx, TUint32 *edx);

CPU::CPU() : BNode("CPU") {
  TUint32 eax, ebx, edx, ecx;
  eax = 0;
  dlog("addresses eax(%x) ebx(%x) ecx(%x) edx(%x)\n", &eax, &ebx, &ecx, &edx);
  dlog("values eax(%x) ebx(%x) ecx(%x) edx(%x)\n", eax, ebx, ecx, edx);
  cpuid(&eax, &ebx, &ecx, &edx);
  dlog("eax(%x) ebx(%x) ecx(%x) edx(%x)\n", eax, ebx, ecx, edx);
  CopyMemory(mManufacturer, (char *)&ebx, 12);
  mManufacturer[12] = '\0';
  dlog("Manufacturer %s\n", mManufacturer);
  bochs;
#if 0
  cpuid(1, &eax, &ebx, &ecx, &edx);
  gCpuInfo.SetProcessorInfo(eax);
  gCpuInfo.mAdditionalInformation = ebx;
  gCpuInfo.mFeatureInformation1 = edx;
  gCpuInfo.mFeatureInformation2 = ecx;

  TUint32 brand[64];
  cpuid(0x80000002, brand + 0x0, brand + 0x1, brand + 0x2, brand + 0x3);
  cpuid(0x80000002, brand + 0x4, brand + 0x5, brand + 0x6, brand + 0x7);
  cpuid(0x80000002, brand + 0x8, brand + 0x9, brand + 0xa, brand + 0xb);
  brand[0x0c] = 0;
  // authenticamd
  // dhexdump32(data, 4);
  bochs;
#endif
}
