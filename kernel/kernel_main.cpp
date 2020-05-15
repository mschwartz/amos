/*
 * main() entrypoint for our example OS-like application
 */

#include <Exec/Types.h>
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

extern "C" int kernel_main(TUint64 ax) {
  extern void *init_start, *init_end,
    *text_start, *text_end,
    *rodata_start, *rodata_end,
    *data_start, *data_end,
    *bss_start, *bss_end,
    *kernel_end;

  in_bochs = *((TUint8 *)0x7c10);

  dlog("Amos V1.0 %s\n",in_bochs ? "BOCHS ENABLED" : "NO BOCHS");
  dlog("         init: %016x - %016x\n", &init_start, &init_end);
  dlog("         text: %016x - %016x\n", &text_start, &text_end);
  dlog("       rodata: %016x - %016x\n", &rodata_start, &rodata_end);
  dlog("         data: %016x - %016x\n", &data_start, &data_end);
  dlog("          bss: %016x - %016x\n", &bss_start, &bss_end);
  dlog("   kernel_end: %016x\n", &kernel_end);
//  dlog("system memory: %d (%d pages)\n", mMMU->total_memory(), mMMU->total_pages());

  //  dlog("bochs %x\n", in_bochs);

//  dputc('B');


  call_global_constructors();

//  dputs("here\n");
//  cli();
//  halt();
  gExecBase.Kickstart();  // does not return

  // it should NEVER get here!
  return 0;
}
