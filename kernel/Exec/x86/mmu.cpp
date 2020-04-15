#include <x86/mmu.h>
#include <x86/bochs.h>
#include <x86/kprint.h>
#include <x86/kernel_memory.h>

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

TUint64 MMU::link_memory_pages(TUint64 address, TUint64 size) {
  if (address == 0) {
    return 0;
  }
  TUint8 *src = (TUint8 *)address;
  TInt pages = size / PAGE_SIZE - 1;
  for (TInt count = 0; count < pages; count++) {
    if ((TUint64)src >= 4 * GIGABYTE) {
      kprint("DONE\n");
      break;
    }
//      dprint("%d link %x %d size:%d pages:%d\r", count, src, src, size, pages);
    TUint64 *ptr = (TUint64 *)&src[0];
//    bzero(src, PAGE_SIZE);
//        memset(src, 0, PAGE_SIZE);

    *ptr = (TUint64)mFreePages;
    mFreePages = src;
    src += PAGE_SIZE;
    //    size -= PAGE_SIZE;
  }
//      dputc('\n');
  return pages;
}

// unlink a 4K page from free list and return it
TAny *MMU::AllocPage() {
  TUint8 *ptr = (TUint8 *)mFreePages;
  TUint64 *ptr64 = (TUint64 *)&ptr[0];
  mFreePages = (TAny *)ptr64[0];
  return (TAny *)ptr;
}

void MMU::FreePage(TAny *aPage) {
  TUint64 *ptr = (TUint64 *)aPage;
  *ptr = (TUint64)mFreePages;
  mFreePages = ptr;
}

// the boot code creates an array of these at a known location, so we can find all the memory.
typedef struct {
  TUint64 address;
  TUint64 size;
  TUint32 type;
  TUint32 acpi;
  void Dump() {
    dprint("  TMemoryInfo: %x address: %x size: $%x(%d) type: %x acpi: %x\n", this, address, size, size, type, acpi);
  }
} PACKED TMemoryInfo;

typedef struct {
  TUint32 mCount;
  TMemoryInfo mInfo[];
  void Dump() {
//    dhexdump((TUint8 *)BIOS_MEMORY, 16);
    dprint("TBiosMemory: %x, %d entries\n", this, mCount);
    for (TInt32 i=0; i<mCount; i++) {
      mInfo[i].Dump();
    }

  }
} PACKED TBiosMemory;

const TUint64 MEMORYTYPE_RANGE = 1;
const TUint64 MEMORYTYPE_RESERVED = 2;

MMU::MMU() {
  mFreePages = nullptr;
  system_memory = 0;
  TBiosMemory *m = (TBiosMemory *)BIOS_MEMORY;
  m->Dump();

  TInt32 count = m->mCount;
#ifdef DEBUGME
  dprint("init_memory %d chunks\n", count);
#endif
  for (TInt32 i=0; i<count; i++) {
    TMemoryInfo *b = &m->mInfo[i]; // defined in memory.inc
    TInt type = b->type;
//    dprint("base: $%x size: %d pages: %d type: %d (%d)\n", b->address, b->size, b->size / PAGE_SIZE, type, MEMORYTYPE_RANGE);
    if (type != 1) {
      continue;
    }
#ifdef DEBUGME
    dprint("->> base: $%x size: $%x(%d) pages: %d type: %d (%d)\n", b->address, b->size, b->size, b->size / PAGE_SIZE, type, MEMORYTYPE_RANGE);
#endif
    system_memory += b->size;
    //            link_memory_pages(b->address, b->size);
    link_memory_pages(b->address, GIGABYTE);
  }
  system_pages = (system_memory + PAGE_SIZE - 1) / PAGE_SIZE;

#if 0
  b->Dump();
  dhexdump((TUint8 *)b, 10);
  TInt type = b->type;
  if (type > 2) {
    type = 0;
  }
  for (TInt i=0; i<count; i++) {
      dprint("base: $%x size: %d pages: %d type: %d (%d)\n", b->address, b->size, b->size / PAGE_SIZE, type, MEMORYTYPE_RANGE);
    if (type == MEMORYTYPE_RANGE) {
//      dprint("RANGE base: $%x size: %d pages: %d type: %d\n", b->address, b->size, b->size / PAGE_SIZE, type);
      system_memory += b->size;
      //            link_memory_pages(b->address, b->size);
      link_memory_pages(b->address, GIGABYTE);
    }
    b = &b[1];
  }
  system_pages = (system_memory + PAGE_SIZE - 1) / PAGE_SIZE;
#endif
 

  // TODO: 64G+ address space mapping
  //  return;
  // blank page directory:
  dprint("blanking page directory\n");
  for (TInt i = 0; i < 512; i++) {
    // kernel-mode access only, write eanbled, not present
    page_directory[i] = 2;
  }

  dprint("set up page tables\n");
  for (TInt t = 0; t < 512; t++) {
    // supervisor level, r/w, present
    page_table[t] = (t * PAGE_SIZE) | 3;
  }

  page_directory[0] = ((TUint64)page_table) | 3;

  dprint("page_directory: %x\n", page_directory);
//  bochs
//  load_page_directory(page_directory);
//  enable_paging();
}

MMU::~MMU() {
}
