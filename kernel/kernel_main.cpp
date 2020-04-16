/*
 * main() entrypoint for our example OS-like application
 */

#include <Exec/BTypes.h>
#include <posix/string.h>
#include <posix/malloc.h>
#include <x86/bochs.h>
#include <x86/kprint.h>
//#include <x86/gdt.h>
//#include <x86/idt.h>
//#include <x86/mmu.h>
#include <Devices/PIC.h>
#include <x86/tasking.h>

#include <Exec/ExecBase.h>

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
    dprint("func %x\n", func);
    (*func)();
  }
}

typedef struct {
  TUint16 mMode;
  TUint16 mWidth;
  TUint16 mHeight;
  TUint16 mPitch;
  TUint16 mBitsPerPixel;
  TUint16 mPad;
  void Dump() {
    dprint("Mode %x %d x %d %d bpp\n", mMode, mWidth, mHeight, mBitsPerPixel);
  }
} PACKED TModeInfo;

typedef struct {
  TInt16 mCount;          // number of modes found
  TModeInfo mDisplayMode; // chosen display mode
  TModeInfo mModes[];
  void Dump() {
    dprint("Found %d %x modes\n", mCount, mCount);
    for (TInt16 i=0; i<mCount; i++) {
      mModes[i].Dump();
    }
  }
} PACKED TModes;

//extern "C" TUint64 __CTOR_LIST__[];
extern "C" int kernel_main(TUint64 ax) {
  in_bochs = *((TUint8 *)0x7c10);
  dprint("bochs %x\n", in_bochs);
  call_global_constructors();

  gExecBase.Hello();

  TModes *modes = (TModes *)0x5000;
  dprint("\n\nDisplay Mode:\n");
  modes->mDisplayMode.Dump();
//  dhexdump((TUint8 *)0x5000, 2);
  dprint("\n\n");
//  kprint("Remaining Modes:\n");
//  modes->Dump();

  Scheduler sc;
  scheduler = &sc;
  kprint("initialized Scheduler\n");

  // set up 8259 PIC
//  PIC p;
  gPIC = new PIC;
  kprint("initialized 8259 PIC\n");
  sti();

  Timer t;
  gTimer = &t;
  kprint("initialized timer\n");

  Keyboard k;
  gKeyboard = &k;
  kprint("initialized keyboard\n");

  gDeviceList.FindDevice("FOO>DEVICE");

//  test_trap();
//  dprint("trap returned\n");
  char buf[10];
  memset(buf, 0, 8);

  extern void *kernel_end;
  dprint("kernel_end = %x\n", &kernel_end);

  char *foo = (char *)malloc(100);
  dprint("AllocMem returned %x\n", foo);

  gExecBase.Reschedule();

  dprint("task0 do nothing\n");
//  char *foo = (char *)malloc(10);
  while (1) {
//    bochs
//    halt();
  }

  return 0;
}
