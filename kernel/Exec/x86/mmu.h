#ifndef KERNEL_MMU_H
#define KERNEL_MMU_H

#include <Exec/Types.h>

const TUint64 PAGE_SIZE = 4096;

typedef struct {
  TUint16 mOffset : 12;
  TUint16 mPML4: 9;          // page map level 4table
  TUint16 mPDP: 9;           // page directory pointer table
  TUint16 mPD :9;            // page directory table
  TUint16 mPT : 9;          // page table
  TUint16 mSign : 16;
} TLogicalAddress;

typedef struct {
  TUint16 mPresent: 1;
  TUint16 mWritable: 1;
  TUint16 mUserAccessible: 1;
  TUint16 mWriteThrough: 1;
  TUint16 mDisableCache: 1;
  TUint16 mAccessed: 1;
  TUint16 mDirty: 1;
  TUint16 mHugePage: 1;
  TUint16 mGlobal: 1;
  TUint16 mUserBits: 9; // available, can be freely used by OS
  TUint64 mPhysicalAddress: 40;
  TUint16 mNoExecute: 1;
} TPageTableEntry;

class MMU {
public:
  MMU();
  ~MMU();

public:
  TUint64 total_memory() { return system_memory; }
  TUint64 total_pages() { return system_pages; }

  TAny *AllocPage();
  void FreePage(TAny *aPage);

protected:
  TUint64 system_memory;
  TUint64 system_pages;
  TAny *mFreePages;

protected:
  TUint64 link_memory_pages(TUint64 address, TUint64 size);
};

#endif
