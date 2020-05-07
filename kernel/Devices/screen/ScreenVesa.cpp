#include <Exec/ExecBase.h>
#include <Devices/screen/ScreenVesa.h>

ScreenVesa::ScreenVesa() {
  dlog("Construct ScreenVesa\n");
  TSystemInfo info;
  gExecBase.GetSystemInfo(&info);

  TUint64 fb = (TUint64)info.mScreenFrameBuffer;
  mBitmap = new BBitmap32(info.mScreenWidth, info.mScreenHeight, info.mScreenPitch, (TAny *)fb);
//  i.Dump();
  mBitmap->Dump();

  TUint32 *pixels = (TUint32 *)fb;
  TUint32 color = 0x7f000000;
  dprint("pixels (%x)\n", pixels);
#if 0
  TInt w = info.mScreenWidth,
       h = info.mScreenHeight,
       x,y;

  for (y = 0; y < h; y++) {
    for (x=0; x < w; x++) {
    dprint("clear %08x x=%d/%d, y=%d/%d\n", color, x, w, y, h);
      *pixels++ = color++;
//      mBitmap->PlotPixel(color++, x, y);
    }
  }

#endif
//  mBitmap->Clear(0xff00ff);
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
