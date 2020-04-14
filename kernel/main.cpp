/*
 * main() entrypoint for our example OS-like application
 */

#include <Exec/BTypes.h>
#include <Exec/ExecBase.h>
#include <posix/string.h>
#include <posix/malloc.h>
#include <x86/bochs.h>
#include <x86/kprint.h>
#include <x86/gdt.h>
#include <x86/idt.h>
#include <x86/mmu.h>
#include <Devices/PIC.h>
//#include <x86/tasking.h>

// devices
#include <Devices/Screen.h>
#include <Devices/Keyboard.h>
#include <Devices/Timer.h>

#include <Exec/Memory.h>
#include <Exec/BDevice.h>

typedef void (*func_ptr)(void);

extern "C" void test_trap();

extern "C" func_ptr __init_array_start[0], __init_array_end[0];
extern "C" func_ptr __global_ctors[0], __global_ctors[0];

static void call_global_constructors(void) {
  dprint("CALL CONSTRUCTORS %x %x\n", __init_array_start, __init_array_end);
  for (func_ptr *func = __init_array_start; func != __init_array_end; func++) {
    dprint("func %x\n", *func);
    (*func)();
  }
}

task_t task0;

extern "C" task_t *current_task;

//extern "C" TUint64 __CTOR_LIST__[];
extern "C" int kernel_main(TUint64 ax) {
  current_task = &task0;

  // logging
  extern void *kernel_end, *init_end, *text_end, *rodata_end, *data_end, *bss_end;
  dprint("\nkernel_end = %x\n", &kernel_end);
  dprint("init_end = %x\n", &init_end);
  dprint("text_end = %x\n", &text_end);
  dprint("rodata_end = %x\n", &rodata_end);
  dprint("data_end = %x\n", &data_end);
  dprint("bss_end = %x\n", &bss_end);

  gScreen = Screen::CreateScreen();
  dprint("initialized screen\n");

  in_bochs = *((TUint8 *)0x7c10);
  dprint("bochs %x\n", in_bochs);
  call_global_constructors();

  kprint("Display Mode:\n");
  dhexdump((TUint8 *)0x5000, 2);
//  Screen::DumpModes();

  GDT g;
  gGDT = &g;
  dprint("initialized GDT\n");

  // set up paging
  MMU m;
  gMMU = &m;
  dprint("initialized MMU\n");

  // set up interrupt handlers
  IDT i;
  gIDT = &i;
  dprint("initialized IDT\n");

//  Scheduler sc;
//  scheduler = &sc;
//  kprint("initialized Scheduler\n");

  CPU _cpu;
  gCPU = &_cpu;

  ExecBase &eb = ExecBase::GetExecBase();
  eb.Init();

  // set up 8259 PIC
  PIC p;
  gPIC = &p;
  kprint("initialized 8259 PIC\n");
  sti();

  Timer t;
  gTimer = &t;
  kprint("initialized timer\n");

  Keyboard k;
  gKeyboard = &k;
  kprint("initialized keyboard\n");

//  gDeviceList.FindDevice("FOO>DEVICE");

//  test_trap();
//  dprint("trap returned\n");
  char buf[10];
  memset(buf, 0, 8);

  char *foo = (char *)malloc(100);
  dprint("AllocMem returned %x\n", foo);

  dprint("task0 do nothing\n");
//  char *foo = (char *)malloc(10);
  while (1) {
//    bochs
//    halt();
  }

  return 0;
}
