/*
 * main() entrypoint for our example OS-like application
 */

#include <Exec/BTypes.h>
#include <x86/bochs.h>
#include <Exec/ExecBase.h>

typedef void (*func_ptr)(void);

// these are for calling global constructors
extern "C" func_ptr __init_array_start[0], __init_array_end[0];

static void call_global_constructors(void) {
//  dprint("CALL CONSTRUCTORS %x %x\n", __init_array_start, __init_array_end);
  for (func_ptr *func = __init_array_start; func != __init_array_end; func++) {
//    dprint("func %x\n", func);
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
    for (TInt16 i = 0; i < mCount; i++) {
      mModes[i].Dump();
    }
  }
} PACKED TModes;

extern "C" int kernel_main(TUint64 ax) {
  call_global_constructors();

  extern void *init_start, *init_end,
    *text_start, *text_end,
    *rodata_start, *rodata_end,
    *data_start, *data_end,
    *bss_start, *bss_end,
    *kernel_end;
  dprint("Amigo V1.0\n");
  dprint("         init: %x - %x\n", &init_start, &init_end);
  dprint("         text: %x - %x\n", &text_start, &text_end);
  dprint("       rodata: %x - %x\n", &rodata_start, &rodata_end);
  dprint("         data: %x - %x\n", &data_start, &data_end);
  dprint("          bss: %x - %x\n", &bss_start, &bss_end);
  dprint("   kernel_end: %x\n", &kernel_end);

  TModes *modes = (TModes *)0x5000;
  dprint("\n\nDisplay Mode:\n");
  modes->mDisplayMode.Dump();
//  gDeviceList.FindDevice("FOO>DEVICE");

  char buf[10];
  memset(buf, 0, 8);

//  gExecBase.Reschedule();
  gExecBase.Kickstart();

  return 0;
}
