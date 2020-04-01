#include <mmu.h>
#include <bochs.h>
#include <kprint.h>
#include <kernel_memory.h>

extern "C" void load_page_directory(TAny *aPtr);
extern "C" void enable_paging();

static TUint64 page_directory[512] __attribute__((aligned(4096)));
static TUint64 page_table[512];

MMU *gMMU;

typedef struct {
  TUint64 address;
  TUint64 size;
  TUint64 type;
  void Dump() {
    dprint("memory_info: %x address: %x size: %d type: %x\n", this, address, size, type);
  }
} PACKED memory_info_t;

const TUint64 MEMORYTYPE_RANGE = 1;
const TUint64 MEMORYTYPE_RESERVED = 2;

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
  int pages = size / PAGE_SIZE - 1;
  for (int count = 0; count < pages; count++) {
    if ((TUint64)src >= 4 * GIGABYTE) {
      kprint("DONE\n");
      break;
    }
//      dprint("%d link %x %d size:%d pages:%d\r", count, src, src, size, pages);
    TUint64 *ptr = (TUint64 *)&src[0];
//    bzero(src, PAGE_SIZE);
//        memset(src, 0, PAGE_SIZE);

    *ptr = (TUint64)free_pages;
    free_pages = src;
    src += PAGE_SIZE;
    //    size -= PAGE_SIZE;
  }
//      dputc('\n');
  return pages;
}

MMU::MMU() {
//  return;
  free_pages = nullptr;
  system_memory = 0;
  TInt16 count = *((TInt16 *)0x9000);
  dprint("init_memory %d chunks\n", count);
  memory_info_t *b = (memory_info_t *)0x9004; // defined in memory.inc
  b->Dump();
  dhexdump((TUint8 *)b, 10);
  int type = b->type;
  if (type > 2) {
    type = 0;
  }
  for (int i=0; i<count; i++) {
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
  // TODO: 64G+ address space mapping
  //  return;
  // blank page directory:
  dprint("blanking page directory\n");
  for (int i = 0; i < 512; i++) {
    // kernel-mode access only, write eanbled, not present
    page_directory[i] = 2;
  }

  dprint("set up page tables\n");
  for (int t = 0; t < 512; t++) {
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
