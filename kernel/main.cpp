/*
 * main() entrypoint for our example OS-like application
 */

#include <Exec/BTypes.h>
#include <posix/string.h>
#include <x86/bochs.h>
#include <x86/kprint.h>
#include <x86/gdt.h>
#include <x86/idt.h>
#include <x86/mmu.h>
#include <Devices/PIC.h>
#include <x86/tasking.h>

// devices
#include <Devices/Screen.h>
#include <Devices/Keyboard.h>
#include <Devices/Timer.h>

#include <Exec/BDevice.h>

typedef void (*func_ptr)(void);

extern "C" void test_trap();

extern "C" func_ptr __init_array_start[0], __init_array_end[0];
extern "C" func_ptr __global_ctors[0], __global_ctors[0];

static void call_global_constructors(void) {
  dprint("CALL CONSTRUCTORS %x %x\n", __init_array_start, __init_array_end);
  for (func_ptr *func = __init_array_start; func != __init_array_end; func++) {
    dprint("func %x\n", func);
    bochs
    (*func)();
  }
}

//extern "C" TUint64 __CTOR_LIST__[];
extern "C" int kernel_main(TUint64 ax) {
  Screen s;
  //  s.putc('X');
  //  s.putc('Y');
  gScreen = &s;
  gScreen->puts("HERE\n");
  dprint("initialized screen\n");
  kprint("initialized screen\n");

  call_global_constructors();

  bochs
  gDeviceList.FindDevice("FOO>DEVICE");


//  for (int i=0; i<10; i++) {
//    dprintf("CTOR %d = %x\n", i, __CTOR_LIST__[i]);
//  }
  GDT g;
  gGDT = &g;
  dprint("initialized GDT\n");
  kprint("initialized GDT\n");

  // set up paging
  MMU m;
  gMMU = &m;
  dprint("initialized MMU\n");
  kprint("initialized MMU\n");

  Scheduler sc;
  scheduler = &sc;
  dprint("initialized Scheduler\n");
  kprint("initialized Scheduler\n");

  // set up interrupt handlers
  IDT i;
  gIDT = &i;
  dprint("initialized IDT\n");
//  kprint("initialized IDT\n");

  CPU _cpu;
  gCPU = &_cpu;

  // set up 8259 PIC
  PIC p;
  gPIC = &p;
  dprint("initialized 8259 PIC\n");
//  kprint("initialized 8259 PIC\n");
  sti();

  Timer t;
  gTimer = &t;
  dprint("initialized timer\n");
//  kprint("initialized timer\n");

  Keyboard k;
  gKeyboard = &k;
  dprint("initialized keyboard\n");
  kprint("initialized keyboard\n");

//  sti();

//  test_trap();
//  dprint("trap returned\n");
  char buf[10];
  memset(buf, 0, 8);

//  halt();
  dprint("task0 do nothing\n");
  while (1) {
//    bochs
//    halt();
  }

  return 0;
}
