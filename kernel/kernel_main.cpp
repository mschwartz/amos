/*
 * main() entrypoint for our example OS-like application
 */

#include <Types.h>
#include <Exec/ExecBase.h>

typedef void (*func_ptr)(void);

extern "C" void sputc(char c);
extern "C" void sputs(const char *s);

// these are for calling global constructors
extern "C" func_ptr __init_array_start[0], __init_array_end[0];

static void call_global_constructors(void) {
//  dlog("about to call global constructors %x %x\n", __init_array_end, __init_array_start);
  for (func_ptr *func = __init_array_start; func != __init_array_end; func++) {
//    dlog("call global constructor %x\n", func);
    (*func)();
//    dlog("  called global constructor %x\n", func);
  }
}

extern "C" TUint64 rdtsc();

extern "C" int kernel_main(TSystemInfo *aSystemInfo) {
  CopyString(&gSystemInfo.mVersion[0], "AMOS v1.0");
  // in_bochs = *((TUint8 *)0x7c10);

  // compute CPU speed
  cli();
  outb(0x43, 0x34);
  outb(0x40, 0);
  outb(0x40, 0);
  TUint64 stsc = rdtsc();
  for (int i=0x1000; i>0; i--);
  TUint64 etsc = rdtsc();
  outb(0x43, 0x04);
  TInt64 lo = inb(0x40);
  TInt64 hi = inb(0x40);
  TInt64 ticks=(0x10000 - (hi*256+lo));
  TInt64 hz = (etsc - stsc) * 1193180 / ticks;
  gSystemInfo.mCpuMhz = hz / 1000000;

  gSystemInfo.mDiskSize = gSystemInfo.mNumHeads * gSystemInfo.mNumSectors * gSystemInfo.mNumCylinders * 512;

  gSystemInfo.Dump();
  // dlog("EBDA\n");
  // dhexdump((TAny *)gSystemInfo.mEBDA, 32);
  call_global_constructors();

  gExecBase.Kickstart();  // does not return

  // it should NEVER get here!
  return 0;
}
