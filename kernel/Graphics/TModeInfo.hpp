#ifndef AMOS_GRAPHICS_TMODEINFO_HPP
#define AMOS_GRAPHICS_TMODEINFO_HPP

#include <Types.hpp>

typedef struct {
  //  TUint16 mPad0;
  TUint32 mMode;
  TUint32 mFrameBuffer;
  TUint32 mWidth;
  TUint32 mHeight;
  TUint32 mPitch;
  TUint32 mDepth;
  TUint32 mPlanes;
  TUint32 mBanks;
  TUint32 mBankSize;
  TUint32 mMemoryModel;
  TUint32 mFrameBufferOffset;
  TUint32 mFrameBufferSize;
  TUint32 mPad2;
  void Dump() {
    dlog("Mode mode(%x) dimensions(%dx%d) depth(%d)  pitch(%d) lfb(0x%x)\n",
      mMode, mWidth, mHeight, mDepth, mPitch, mFrameBuffer);
  }
} PACKED TModeInfo;

typedef struct {
  TInt32 mCount;          // number of modes found
  TModeInfo mDisplayMode; // chosen display mode
  TModeInfo mModes[];
  void Dump() {
    dlog("Found %d %x modes\n", mCount, mCount);
    for (TInt16 i = 0; i < mCount; i++) {
      mModes[i].Dump();
    }
  }
} PACKED TModes;

static TModes *gGraphicsModes = (TModes *)0xa000;

#endif
