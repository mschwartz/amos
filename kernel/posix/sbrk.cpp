#include <unistd.h>
#ifdef KERNEL
#include <Exec/ExecBase.h>
#endif

extern void *kernel_end;
static uint8_t *gProgramBreak = 0;

// sets end of the data segment to the value specified by addr
extern "C" int brk(void *aAddress) {
#ifdef KERNEL
  dlog("brk called %x\n", aAddress);
  bochs
#endif
  return 0;
}

// increments the programs data space by increment bytes
extern "C" void *sbrk(intptr_t aIncrement) {
  if (gProgramBreak == nullptr) {
    gProgramBreak = (uint8_t *)&kernel_end;
  }
  if (aIncrement == 0) {
    return gProgramBreak;
  }

  void *ret = gProgramBreak;
  gProgramBreak += aIncrement;
  return ret;
}

