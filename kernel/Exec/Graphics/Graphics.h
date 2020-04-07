#ifndef EXEX_GRAPHICS_GRAPHICS_H
#define EXEX_GRAPHICS_GRAPHICS_H

// Master Graphics include file - baseically defines all graphics methods and types.

#include <BTypes.h>
#include <Graphics/TRGB.h>
#include <Graphics/TRect.h>
#include <Graphics/TBCD.h>
#include <Graphics/TNumber.h>

#include <Graphics/BBitmap.h>
#include <Graphics/BFont.h>

#include <x86/bochs.h>

#if 0
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
static TModes *gDisplayModes = (TModes *)0x5000;
#endif

#endif
