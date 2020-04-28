#include <Devices/screen/ScreenVesa.h>
#include <Exec/x86/bochs.h>

typedef struct {
  TUint16 mPad0;
  TUint16 mMode;
  TUint32 mFrameBuffer;
  TUint16 mPad1;
  TUint16 mWidth;
  TUint16 mHeight;
  TUint16 mPitch;
  TUint16 mDepth;
  TUint16 mPad2;
  void Dump() {
    dlog("Mode %x %d x %d %d bpp pitch %d frame buffer at 0x%x\n",  mMode, mWidth, mHeight, mDepth, mPitch, mFrameBuffer);
  }
} PACKED TModeInfo;

typedef struct {
  TInt16 mCount;          // number of modes found
  TModeInfo mDisplayMode; // chosen display mode
  TModeInfo mModes[];
  void Dump() {
    dlog("Found %d %x modes\n", mCount, mCount);
    for (TInt16 i = 0; i < mCount; i++) {
      mModes[i].Dump();
    }
  }
} PACKED TModes;

ScreenVesa::ScreenVesa() {
  TModes *modes = (TModes *)0x5000;
  dlog("\n\nDisplay Mode:\n");
  modes->mDisplayMode.Dump();

  // fill screen with ff00ff
  TModeInfo &i = modes->mDisplayMode;

  TUint64 fb = (TUint64)i.mFrameBuffer;
  mBitmap = new BBitmap32(i.mWidth, i.mHeight, i.mPitch, (TAny *)fb);
  mBitmap->Clear(0xff00ff);
}

void ScreenVesa::MoveTo(int aX, int aY) {
  mX = aX;
  mY = aY;
}

void ScreenVesa::GetXY(TInt& aX, TInt& aY) {
  aX = mX;
  aY = mY;
}

void ScreenVesa::ClearEOL(TUint8 aCharacter) {
  //
}

void ScreenVesa::Down() {
  //
}

void ScreenVesa::ScrollUp() {
  //
}

void ScreenVesa::NewLine() {
  //
}

void ScreenVesa::WriteChar(char c) {
  //
}

void ScreenVesa::ClearScreen(TUint8 ch) {
  //
}

void ScreenVesa::WriteString(const char *s) {
  //
}
