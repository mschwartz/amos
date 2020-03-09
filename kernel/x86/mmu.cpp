#include <mmu.h>
#include <bochs.h>
#include <kprint.h>
#include <kernel_memory.h>

extern "C" void load_page_directory(void *ptr);
extern "C" void enable_paging();

static uint64_t page_directory[1024] __attribute__((aligned(4096)));
static uint64_t page_table[1024];

MMU *mmu;

struct MEMORY_INFO {
  uint64_t address;
  uint64_t size;
  uint64_t type;
} PACKED;

const uint64_t MEMORYTYPE_RANGE = 1;
const uint64_t MEMORYTYPE_RESERVED = 2;

static const char *types[] = {
  "Undefined",
  "Range    ",
  "Reserved ",
};

uint64_t MMU::link_memory_pages(uint64_t address, uint64_t size) {
  if (address == 0) {
    return 0;
  }
  uint8_t *src = (uint8_t *)address;
  int pages = size / PAGE_SIZE - 1;
  for (int count = 0; count < pages; count++) {
    if ((uint64_t)src >= 0x200000) {
      kprint("DONE\n");
      break;
    }
    //  dprint("%d link %x %d size:%d pages:%d\n", count, src, src, size, pages);
    uint64_t *ptr = (uint64_t *)&src[0];
    bzero(src, PAGE_SIZE);
    //    memset(src, 0, PAGE_SIZE);

    *ptr = (uint64_t)free_pages;
    free_pages = src;
    src += PAGE_SIZE;
    //    size -= PAGE_SIZE;
  }
  //  while (1);
  return pages;
}

MMU::MMU() {
  return;
  free_pages = nullptr;
  system_memory = 0;
  kprint("init_memory\n");
  MEMORY_INFO *b = (MEMORY_INFO *)0x5000; // defined in memory.inc
  int type = b->type;
  if (type > 2) {
    type = 0;
  }
  while (b->address || b->size || b->type) {
    if (b->type == MEMORYTYPE_RANGE) {
      system_memory += b->size;
      kprint("base: $%x size: %d pages: %d type: %s\n", b->address, b->size, b->size / PAGE_SIZE, types[type]);
      //            link_memory_pages(b->address, b->size);
      link_memory_pages(b->address, GIGABYTE);
    }
    b = &b[1];
  }
  system_pages = (system_memory + PAGE_SIZE - 1) / PAGE_SIZE;
  // TODO: 64G+ address space mapping
  //  return;
  // blank page directory:
  kprint("blanking page directory\n");
  for (int i = 0; i < 1024; i++) {
    // kernel-mode access only, write eanbled, not present
    page_directory[i] = 2;
  }

  kprint("set up page tables\n");
  for (int t = 0; t < 1024; t++) {
    // supervisor level, r/w, present
    page_table[t] = (t * PAGE_SIZE) | 3;
  }

  page_directory[0] = ((uint64_t)page_table) | 3;

  dprint("page_directory: %x\n", page_directory);
//  bochs
//  load_page_directory(page_directory);
//  enable_paging();
}

MMU::~MMU() {
}
