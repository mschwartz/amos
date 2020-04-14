#include <Exec/Devices/Screen.h>
#include <x86/cpu.h>
#include <x86/bochs.h>
#include <x86/kprint.h>
#include <x86/kernel_memory.h>
#include "screen/VGAScreen.h"
#include "screen/BitmapScreen.h"

typedef struct {
  char mSignature[4];
  TUint16 mVersion;
  TUint32 mOem;
  TUint32 mCapabilities;
  TUint32 *mVideoModes;
  TUint16 mVideoMemory;
  TUint16 mSoftwareRev;
  TUint32 mVendor;
  TUint32 mProductName;
  TUint32 mProductRev;
  TUint8 reserved[222];
  char mOemData[256];
  void Dump() {
    dprint("DisplayInfo @ 0x%x\n", this);
    dprint("  mSignature:    %c%c%c%c\n", mSignature[0], mSignature[1], mSignature[2], mSignature[3]);
    dprint("  mVersion:      0x%x\n", mVersion);
    dprint("  mOem:          0x%x\n", mOem);
    dprint("  mCapabilities: 0x%x\n", mCapabilities);
    dprint("  mVideoModes:   0x%x\n", mVideoModes);
    dprint("  mVideoMemory:  0x%x\n", mVideoMemory);
    dprint("  mSoftwareRev:  0x%x\n", mSoftwareRev);
    dprint("  mVendor:       0x%x\n", mVendor);
    dprint("  mProductName:  0x%x\n", mProductName);
    dprint("  mProductRev:   0x%x\n", mProductRev);
    dprint(" %d 0x%x\n\n", sizeof(*this), sizeof(*this));
  }

} PACKED TDisplayInfo;

typedef struct {
  TUint16 mMode;
  TUint16 mWidth;
  TUint16 mHeight;
  TUint16 mDepth;
  TUint16 mPitch;
  TUint16 mPad;
  TUint32 mFrameBuffer;
  TUint32 mFrameBufferOffset;
  TUint16 mFrameBufferSize;
  TUint16 mPad2;
  void Dump() {
    dprint("ModeInfo @ 0x%x\n", this);
    dprint("  mMode:              %x 0x%x\n", &mMode, mMode);
    dprint("  mWidth:             %x %d\n", &mWidth, mWidth);
    dprint("  mHeight:            %x %d\n", &mHeight, mHeight);
    dprint("  mDepth:             %x %d\n", &mDepth, mDepth);
    dprint("  mPitch:             %x %d\n", &mPitch, mPitch);
    dprint("  mFrameBuffer:       %x 0x%x\n", &mFrameBuffer, mFrameBuffer);
    dprint("  mFrameBufferOffset: %x 0x%x\n", &mFrameBufferOffset, mFrameBufferOffset);
    dprint("  mFrameBufferSize:   %x 0x%x\n", &mFrameBufferSize, mFrameBufferSize);
    dprint(" %d 0x%x\n\n", sizeof(*this), sizeof(*this));
//    dprint("Mode %x %d x %d %d bpp 0x%x + 0x%x %x(%d)\n", mMode, mWidth, mHeight, mBitsPerPixel, mFrameBuffer, mFrameBufferOffset, mFrameBufferSize, mFrameBufferSize);
  }
} PACKED TModeInfo;

typedef struct {
  TInt32 mCount;          // number of modes found
  TDisplayInfo mDisplayInfo;
  TModeInfo mDisplayMode; // chosen display mode
  TModeInfo mModes[];
  void Dump() {
    dprint("VESA info\n");
//    dprint("mCount: %x\n", &mCount);
//    dprint("mDisplayInfo: %x\n", &mDisplayInfo);
//    dprint("mDisplayMode: %x\n", &mDisplayMode);
//    dprint("mModes: %x\n", &mModes[0]);
//    return;

    mDisplayInfo.Dump();
    dprint("Selected Mode\n");
    mDisplayMode.Dump();
    dprint("Found %d %x modes\n", mCount, mCount);
    for (TInt16 i = 0; i < mCount; i++) {
      mModes[i].Dump();
    }
  }
} PACKED TModes;

static TModes *gDisplayModes = (TModes *)0x5000;


Screen *Screen::CreateScreen() {

#ifdef KGFX
  dprint("Create Bitmap Screen\n");
  return new BitmapScreen();
#else
  dprint("Create VGA Screen\n");
  return new VGAScreen();
#endif
}

Screen::Screen() {
  TModeInfo *mode = &gDisplayModes->mDisplayMode;
  mDisplayMode.mWidth = mode->mWidth;
  mDisplayMode.mHeight = mode->mHeight;
  mDisplayMode.mDepth = mode->mDepth;
  mDisplayMode.mPitch = mode->mPitch;
  mDisplayMode.mFrameBuffer = (void *)mode->mFrameBuffer;
}

Screen::~Screen() {
  //
}

void Screen::hexnybble(const TUint8 n) {
  const char *nybbles = "0123456789ABCDEF";
  putc(nybbles[n & 0x0f]);
}

void Screen::hexbyte(const TUint8 b) {
  hexnybble(b >> 4);
  hexnybble(b);
}

void Screen::hexword(const TUint16 w) {
  hexbyte(w >> 8);
  hexbyte(w);
}

void Screen::hexlong(const TUint32 l) {
  hexword((l >> 16) & 0xffff);
  hexword(l & 0xffff);
}

void Screen::hexquad(const TUint64 l) {
  hexlong((l >> 32) & 0xffffffff);
  hexlong(l & 0xffffffff);
}

void Screen::hexdump(const TAny *addr, TInt count) {
  TUint8 *ptr = (TUint8 *)addr;
  TUint64 address = (TUint64)addr;;
  hexlong(address);
  putc(' ');
  for (TInt i = 0; i < count; i++) {
    hexbyte(*ptr++);
    putc(' ');
  }
  newline();
}

void Screen::DumpModes() {
  gDisplayModes->Dump();
}

Screen *gScreen;
