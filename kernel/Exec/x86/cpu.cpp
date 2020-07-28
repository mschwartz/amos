#include "cpu.hpp"

extern "C" TUint64 cpuid(TUint32 *eax, TUint32 *ebx, TUint32 *ecx, TUint32 *edx);

static void copy_memory(TAny *dst, TAny *src, TInt count) {
  dlog("copy dst(%x) src(%x) count(%d)\n", dst, src, count);
  TUint8 *s = (TUint8 *)src;
  TUint8 *d = (TUint8 *)dst;
  for (TInt i = 0; i < count; i++) {
    *d++ = *s++;
  }
}
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
  mMaxFunction = eax;
  dlog("mMaxFunction: %02x\n", mMaxFunction);

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

  // SetMemory16(

  TUint32 *brand = (TUint32 *)&mBrand[0];
  dlog("allocated &mBrand(%x) mBrand(%x) brand(%x)\n", &mBrand, mBrand, brand);
  mBrand[48] = '\0';
  dlog("allocated &mBrand(%x) mBrand(%x) brand(%x)\n", &mBrand, mBrand, brand);
  
  eax = 0x80000002;
  cpuid(&eax, &ebx, &ecx, &edx);
  dlog("values eax(%x) ebx(%x) ecx(%x) edx(%x)\n", eax, ebx, ecx, edx);
  dlog("allocated &mBrand(%x) mBrand(%x) brand(%x)\n", &mBrand, mBrand, brand);
  *brand++ = eax;
  dlog("allocated &mBrand(%x) mBrand(%x) brand(%x)\n", &mBrand, mBrand, brand);
  *brand++ = ebx;
  dlog("allocated &mBrand(%x) mBrand(%x) brand(%x)\n", &mBrand, mBrand, brand);
  *brand++ = ecx;
  dlog("allocated &mBrand(%x) mBrand(%x) brand(%x)\n", &mBrand, mBrand, brand);
  *brand++ = edx;
  dlog("allocated &mBrand(%x) mBrand(%x) brand(%x)\n", &mBrand, mBrand, brand);
  bochs;

  eax = 0x80000003;
  cpuid(&eax, &ebx, &ecx, &edx);
  dlog("values eax(%x) ebx(%x) ecx(%x) edx(%x)\n", eax, ebx, ecx, edx);
  *brand++ = eax;
  *brand++ = ebx;
  *brand++ = ecx;
  *brand++ = edx;
  dlog("allocated mBrand(%x)\n", mBrand);

  eax = 0x80000004;
  cpuid(&eax, &ebx, &ecx, &edx);
  dlog("values eax(%x) ebx(%x) ecx(%x) edx(%x)\n", eax, ebx, ecx, edx);
  *brand++ = eax;
  *brand++ = ebx;
  *brand++ = ecx;
  *brand++ = edx;

  dlog("Brand: %x(%s)\n", mBrand, mBrand);
  dhexdump(mBrand, 8);

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
