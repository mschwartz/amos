#include <kernel_memory.h>
//#include <kernel_interrupts.h>
#include <Screen.h>
#include <kprint.h>
#include <bochs.h>

#if 0
struct MEMORY_INFO {
  uint64_t address;
  uint64_t size;
  uint64_t type;
} PACKED;

static uint8_t *free_pages;

const uint64_t MEMORYTYPE_RANGE = 1;
const uint64_t MEMORYTYPE_RESERVED = 2;

static const char *types[] = {
  "Undefined",
  "Range    ",
  "Reserved ",
};

uint64_t link_memory_pages(uint64_t address, uint64_t size) {
  if (address == 0) {
    return 0;
  }
  uint8_t *src = (uint8_t *)address;
  int pages = size / PAGE_SIZE - 1;
  for (int count = 0; count < pages; count++) {
    if ((uint64_t)src >= 0x200000) {
      screen->puts("DONE\n");
      break;
    }
    //    screen->putc('.');
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

uint64_t init_memory() {
//  bochs
  //  init_interrupts();
  //  disable_interrupts();
  uint64_t size = 0;
  screen->puts("init_memory\n");
  free_pages = nullptr;
  MEMORY_INFO *b = (MEMORY_INFO *)0x5000; // defined in memory.inc
  int type = b->type;
  if (type > 2) {
    type = 0;
  }
  while (b->address || b->size || b->type) {
    if (b->type == MEMORYTYPE_RANGE) {
      size += b->size;
      kprint("base: $%x size: %d pages: %d type: %s\n", b->address, b->size, b->size / PAGE_SIZE, types[type]);
//            link_memory_pages(b->address, b->size);
      link_memory_pages(b->address, GIGABYTE);
    }
    b = &b[1];
  }
  return size;
}
#endif

