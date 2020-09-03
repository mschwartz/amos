#ifndef AMOS_EXEC_X86_PAGETABLE_HPP
#define AMOS_EXEC_X86_PAGETABLE_HPP

#include <Exec/ExecBase.hpp>
#include <Exec/x86/mmu.hpp>

const TUint64 PAGE_SIZE = 4096;

// values for aFlags in MapPage
const TUint16 PT_PRESENT = (1 << 0);      // if set, page is present
const TUint16 PT_WRITABLE = (1 << 1);     // if set, page is writable
const TUint16 PT_USER = (1 << 2);         // if set, page can be accessed by user priv
const TUint16 PT_WRITETHROUGH = (1 << 3); // if set, write-through caching
const TUint16 PT_DISABLE = (1 << 4);      // if set, page will not be cached
const TUint16 PT_ACCESSED = (1 << 5);     // set by CPU when page is accessed
const TUint16 PT_PAGESIZE4MB = (1 << 7);  // if set, pagesize is 4M, otherwise it is 4K
const TUint16 PT_DEFAULTS = PT_PRESENT | PT_WRITABLE | PT_USER;

class PageTable : public BBase {
public:
  PageTable(MMU *aMmu);
public:
  void Dump();
  
public:
  TUint64 *AllocPage();
  void MapPage(TUint64 aVirtualAddress, TUint64 aPhysicalAddress, TUint16 aFlags = PT_DEFAULTS);
  TUint64 PhysicalAddress(TUint64 aVirtualAddress);
  TUint64 VirtualAddress(TUint64 aPhysicalAddress);

public:
  TUint64 *mPML4;

protected:
  MMU *mMmu;
  TUint64 mPageCount;
};

#endif
