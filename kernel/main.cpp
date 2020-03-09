/*
 * main() entrypoint for our example OS-like application
 */

#include <types.h>
#include <string.h>
#include <bochs.h>
#include <kprint.h>
#include <gdt.h>
#include <idt.h>
#include <mmu.h>
#include <PIC.h>
#include <tasking.h>

// devices
#include <Screen.h>
#include <Keyboard.h>
#include <Timer.h>

extern "C" void test_trap();

extern "C" int kernel_main(uint32_t ax) {
  Screen s;
  //  s.putc('X');
  //  s.putc('Y');
  screen = &s;
  screen->puts("HERE\n");
  dprint("initialized screen\n");
  kprint("initialized screen\n");

  GDT g;
  gdt = &g;
  dprint("initialized GDT\n");
  kprint("initialized GDT\n");

  // set up paging
  MMU m;
  mmu = &m;
  dprint("initialized MMU\n");
  kprint("initialized MMU\n");

  Scheduler sc;
  scheduler = &sc;
  dprint("initialized Scheduler\n");
  kprint("initialized Scheduler\n");


  // set up interrupt handlers
  IDT i;
  idt = &i;
  dprint("initialized IDT\n");
//  kprint("initialized IDT\n");

  CPU _cpu;
  cpu = &_cpu;
  // set up 8259 PIC
  PIC p;
  pic = &p;
  dprint("initialized 8259 PIC\n");
//  kprint("initialized 8259 PIC\n");
  sti();

  Timer t;
  timer = &t;
  dprint("initialized timer\n");
//  kprint("initialized timer\n");

  Keyboard k;
  keyboard = &k;
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
