#include <Exec/x86/mmu.hpp>
#include <Exec/x86/kernel_memory.hpp>

#define DEBUGME
#undef DEBUGME

extern "C" void load_page_directory(TAny *aPtr);
extern "C" void enable_paging();

static TUint64 page_directory[512] __attribute__((aligned(4096)));
static TUint64 page_table[512];

static const char *types[] = {
  "Undefined",
  "Range    ",
  "Reserved ",
};

extern "C" void *kernel_end;

TUint64 MMU::link_memory_pages(TUint64 address, TUint64 size) {
  const TUint64 start = ((TUint64)&kernel_end) + 200 * MEGABYTE;

  dlog("link_memory_pages(%x, %d/%x) start(%x)\n", address, size, size, start);
  if (address == 0) {
    return 0;
  }
  dlog("  link_memory_pages(%x, %d/%x)\n", address, size, size);
  TUint8 *src = (TUint8 *)address;
  TInt pages = size / PAGE_SIZE - 1;
  for (TInt count = 0; count < pages; count++) {
    // if ((TUint64)src >= 4 * GIGABYTE) {
    //   //      kprint("DONE\n");
    //   break;
    // }
    TUint64 *ptr = (TUint64 *)&src[0];

    if (address >= start) {
      // dlog("   address(%x) start(%x) src(%x)\n", address, start, src);
      // dlog("    %d link %x %d size:%d pages:%d\n", count, src, src, size, pages);
      //    bzero(src, PAGE_SIZE);
      //        memset(src, 0, PAGE_SIZE);
      *ptr = (TUint64)mFreePages;
      mFreePages = src;
    }
    address += PAGE_SIZE;
    system_memory += PAGE_SIZE;
    src += PAGE_SIZE;
    if (src > mHighAddress) {
      mHighAddress = src;
    }
    //    size -= PAGE_SIZE;
  }
  // dputc('\n');
  return pages;
}

// unlink a 4K page from free list and return it
TAny *MMU::AllocPage() {
  // dprint("-------------------- MMU AllocPage %x ", mFreePages);
  TUint8 *ptr = (TUint8 *)mFreePages;
  TUint64 *ptr64 = (TUint64 *)&ptr[0];
  mFreePages = (TAny *)ptr64[0];
  // dprint("ptr(%x) mFreePages(%x)\n", ptr, mFreePages);
  return (TAny *)ptr;
}

void MMU::FreePage(TAny *aPage) {
  TUint64 *ptr = (TUint64 *)aPage;
  *ptr = (TUint64)mFreePages;
  mFreePages = ptr;
}

MMU::MMU() {
  mFreePages = nullptr;
  mHighAddress = nullptr;
  system_memory = 0;
  TBiosMemory *m = (TBiosMemory *)BIOS_MEMORY;
  m->Dump();
  dprint("\n\n");

  TInt32 count = m->mCount;
  for (TInt32 i = 0; i < count; i++) {
    TMemoryInfo *b = &m->mInfo[i]; // defined in memory.inc
    TInt type = b->type;
    if (type != 1) {
      continue;
    }
    // b->Dump();
    link_memory_pages(b->address, b->size);
  }
  system_pages = (system_memory + PAGE_SIZE - 1) / PAGE_SIZE;

  // gSystemInfo.mRam = HighAddress();

  gSystemInfo.Dump();
  
  dprint("system_memory(%d) system_pages(%d)\n", system_memory, system_pages);
  return;
  
  bochs;
  PageTable *pt = new PageTable(this);
  for (TUint64 address = 0; address < 8 * GIGABYTE; address++) {
    pt->MapPage(address, address);
  }
  bochs;
  pt->Dump();
  return;

  // TODO: 64G+ address space mapping
  //  return;

  // blank page directory:
#ifdef DEBUGME
  dlog("blanking page directory\n");
#endif
  for (TInt i = 0; i < 512; i++) {
    // kernel-mode access only, write eanbled, not present
    page_directory[i] = 2;
  }

#ifdef DEBUGME
  dlog("set up page tables\n");
#endif
  for (TInt t = 0; t < 512; t++) {
    // supervisor level, r/w, present
    page_table[t] = (t * PAGE_SIZE) | 3;
  }

  page_directory[0] = ((TUint64)page_table) | 3;

#ifdef DEBUGME
  dlog("page_directory: %x\n", page_directory);
#endif
  //  bochs
  //  load_page_directory(page_directory);
  //  enable_paging();
}

MMU::~MMU() {
}
