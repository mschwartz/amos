/*!
 * 6845 CRT Controller
 */

#ifndef SCREEN_H
#define SCREEN_H

#include <Exec/BTypes.h>
#include <Exec/BBase.h>
#include <Exec/Graphics/BBitmap.h>
#include <Exec/Memory.h>
#include <x86/bochs.h>

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

class Screen : public BBase {
public:
  // factory
  static Screen *CreateScreen();

public:
//  Screen(); //   {}
//  virtual ~Screen() {}

public:
  virtual void moveto(int x, int y) = 0;
  virtual void getxy(int &x, int &y) = 0;
  virtual void down();

  // clear to end of line.  cursor is not moved.
  virtual void cleareol(TUint8 ch = ' ') = 0;

  // scroll screen up one line starting at row y.  Fill in bottom row with bTInt
  virtual void scrollup() = 0;

  virtual void newline() = 0;
  virtual void putc(char c) = 0;
  virtual void cls(TUint8 ch = ' ') = 0;
  virtual void puts(const char *s) = 0;

   void hexnybble(const TUint8 n);
   void hexbyte(const TUint8 b) ;
   void hexword(const TUint16 w);
   void hexlong(const TUint32 l);
   void hexquad(const TUint64 l);
   void hexdump(const void *addr, TInt count);

protected:
  TUint8 *screen;
  TUint8 attribute;
  TInt16 row, col;
  char buf[256];
};

extern Screen *gScreen;

#endif
