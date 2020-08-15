/*
 * main() entrypoint for our example OS-like application
 */

#include <Types.hpp>
#include <Exec/ExecBase.hpp>

typedef void (*func_ptr)(void);

extern "C" void sputc(char c);
extern "C" void sputs(const char *s);

// these are for calling global constructors
extern "C" func_ptr __init_array_start[0], __init_array_end[0];

static void call_global_constructors(void) {
  // dlog("about to call global constructors %x %x\n", __init_array_end, __init_array_start);
  for (func_ptr *func = __init_array_start; func != __init_array_end; func++) {
    // dlog("call global constructor %x\n", func);
    (*func)();
    // dlog("  called global constructor %x\n", func);
  }
}

extern "C" void SetRSP(TUint8 *sp);

/**
 * ap_main(aCpuNumber);
 * 
 * This is the boot/start entry point for C++ kernel for an AP.  The BSP (CPU 0) has
 * kernel_main() as its entry point.  The BSP has to initialize all sorts of things, including
 * ACPI, PCI, interrupts, IDT, ExecBase, etc.  
 *
 * The BSP also starts the APs, one at a time.  It is important that this process waits for the
 * CPU's mCpuState to become ECpuRunning.  This is set in IdleTask, to assure that the CPU
 * is not only booted, but also in tasking (running a task/IdleTask).
 *
 * The boot.asm and kernel_start.asm code uses the same stack for each CPU as it boots.  We don't
 * want some race condition where two CPUs are using the same stack memory.  
 *
 * The scheme described allows each AP to use the boot stack until it enters tasking mode, then
 * the next AP gets to start.
 */
extern "C" int ap_main(TInt64 aCpuNumber) {
  cli();
  // static TInt64 a;
  // CPU *cpu;
  // a = aCpuNumber;
  CPU *cpu = gExecBase.GetCpu(aCpuNumber);
  cpu->EnterAP();
  while (1) {
    halt();
  }
  return 0;
}

extern "C" TUint64 rdtsc();

extern "C" int kernel_main(TSystemInfo *aSystemInfo) {
  cli();
  // dhexdump((TAny *)0x8000, 10);

  InitAllocMem();
  CopyString(&gSystemInfo.mVersion[0], "AMOS v1.0");
  // in_bochs = *((TUint8 *)0x7c10);

  // compute CPU speed
  outb(0x43, 0x34);
  outb(0x40, 0);
  outb(0x40, 0);
  TUint64 stsc = rdtsc();
  for (int i = 0x1000; i > 0; i--)
    ;
  TUint64 etsc = rdtsc();
  outb(0x43, 0x04);
  TInt64 lo = inb(0x40);
  TInt64 hi = inb(0x40);
  TInt64 ticks = (0x10000 - (hi * 256 + lo));
  TInt64 hz = (etsc - stsc) * 1193180 / ticks;
  gSystemInfo.mCpuMhz = hz / 1000000;

  gSystemInfo.mDiskSize = gSystemInfo.mNumHeads * gSystemInfo.mNumSectors * gSystemInfo.mNumCylinders * 512;

  call_global_constructors();

  // it should NEVER get here!
  bochs;
  return 0;
}
