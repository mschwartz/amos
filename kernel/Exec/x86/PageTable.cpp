#include <Exec/x86/PageTable.hpp>

PageTable::PageTable(MMU *aMmu) : BBase() {
  mMmu = aMmu;
  mPML4 = (TUint64 *)AllocPage();
  mPageCount = 0;
  // TODO: error handling
}

TUint64 *PageTable::AllocPage() {
  mPageCount++;
  TUint64 *ptr = (TUint64 *)mMmu->AllocPage();
  SetMemory64(ptr, 0, 512);
  return ptr;
}

void PageTable::MapPage(TUint64 aVirtualAddress, TUint64 aPhysicalAddress, TUint16 aFlags) {
  TUint64 l4 = ((aVirtualAddress >> 39) & 0x1ff),
          l3 = ((aVirtualAddress >> 29) & 0x1ff),
          l2 = ((aVirtualAddress >> 21) & 0x1ff),
          l1 = ((aVirtualAddress >> 12) & 0x1ff);

  // dprint("MapPage(%x, %x, %x) l4(%x) l3(%x) l2(%x) l1(%x)\n",
  //   aVirtualAddress, aPhysicalAddress, aFlags, l4, l3, l2, l1);
  if (mPML4[l4] == 0) {
    mPML4[l4] = (TUint64)AllocPage() | aFlags;
    dprint("  allocate pml4[%x]\n", l4);
    // bochs;
  }
  TUint64 *p3 = (TUint64 *)(mPML4[l4] & ~0xfff);

  if (p3[l3] == 0) {
    p3[l3] = (TUint64)AllocPage() | aFlags;
    dprint("  allocate p3[%x]\n", l3);
    // bochs;
  }
  TUint64 *p2 = (TUint64 *)(p3[l3] & ~0xfff);

  if (p2[l2] == 0) {
    p2[l2] = (TUint64)AllocPage() | aFlags;
    dprint("  allocate p2[%x] l4(%x) l3(%x) virtual(%x)\n", l2, l4, l3, aVirtualAddress);
    // bochs;
  }
  TUint64 *p1 = (TUint64 *)(p2[l2] & ~0xfff);
  p1[l1] = aPhysicalAddress | aFlags;
}

TUint64 PageTable::PhysicalAddress(TUint64 aVirtualAddress) {
  TUint64 l4 = (aVirtualAddress >> 39) & ~0xfff,
          l3 = (aVirtualAddress >> 29) & ~0xfff,
          l2 = (aVirtualAddress >> 21) & ~0xfff,
          l1 = (aVirtualAddress >> 12) & ~0xfff;

  if (mPML4[l4] == 0) {
    return ~0;
  }
  TUint64 *p3 = (TUint64 *)(mPML4[l4] & ~0xfff);

  if (p3[l3] == 0) {
    return ~0;
  }
  TUint64 *p2 = (TUint64 *)(p3[l3] & ~0xfff);

  if (p2[l2] == 0) {
    return ~0;
  }
  TUint64 *p1 = (TUint64 *)(p2[l2] & ~0xfff);
  if (p1[l1] == 0) {
    return ~0;
  }
  return p1[l1] & ~0xfff;
}

TUint64 PageTable::VirtualAddress(TUint64 aPhysicalAddress) {
  return 0;
}

void PageTable::Dump() {
  dprint("PageTable (%x)\n", this);

  TUint64 *ptr = (TUint64 *)mPML4;

  for (TInt i = 0; i < 512; i++) {
    if ((i % 4) == 0) {
      dprint("\n%016x    ", &ptr[i]);
    }
    dprint("%016x  ", ptr[i]);
  }
  dprint("\n\n");

  ptr = (TUint64 *)(ptr[0] & ~0xfff);

  for (TInt i = 0; i < 512; i++) {
    if ((i % 4) == 0) {
      dprint("\n%016x    ", &ptr[i]);
    }
    dprint("%016x  ", ptr[i]);
  }
  dprint("\n\n");

  ptr = (TUint64 *)(ptr[0] & ~0xfff);

  for (TInt i = 0; i < 512; i++) {
    if ((i % 4) == 0) {
      dprint("\n%016x    ", &ptr[i]);
    }
    dprint("%016x  ", ptr[i]);
  }
  dprint("\n");
}
