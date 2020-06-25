#include <Exec/ExecBase.h>
#include <Inspiration/Display.h>

Display::Display() : BNode("Display") {
  dlog("Construct Display\n");
  TSystemInfo info;
  gExecBase.GetSystemInfo(&info);

  TUint64 fb = (TUint64)info.mDisplayFrameBuffer;
  mBitmap = new BBitmap32(info.mDisplayWidth, info.mDisplayHeight, info.mDisplayPitch, (TAny *)fb);
  //  i.Dump();
  //  mBitmap->Dump();
  mMouseX = mMouseY = -1;
  mMouseHidden = ETrue;
  MoveCursor(20, 20);
  ShowCursor();
}

#include <Inspiration/cursors/pointer.h>

static void render_cursor(BBitmap32 *aBitmap, TInt aX, TInt aY) {
//  dlog("render_cursor %d,%d\n", aX, aY);
  TUint8 *img = cursor_image;
  aX -= 8;
  aY -= 8;
  for (TInt y = 0; y < 16; y++) {
    for (TInt x = 0; x < 16; x++) {
      if (img[x]) {
        if (aBitmap->PointInRect(aX + x, aY + y)) {
          TUint32 color = aBitmap->ReadPixel(aX + x, aY + y);
          color ^= 0xffffffff;
          aBitmap->PlotPixel(color, aX + x, aY + y);
        }
      }
    }
    img += 16;
  }
  //  const TInt WIDTH = 32, HEIGHT = 32;
  //  const TInt x1 = aX - WIDTH / 2,
  //             y1 = aY - HEIGHT / 2,
  //             x2 = aX + WIDTH / 2,
  //             y2 = aY + HEIGHT / 2;

  //  for (TInt y = y1; y < y2; y++) {
  //    for (TInt x = x1; x < x2; x++) {
  //      if (aBitmap->PointInRect(x, y)) {
  //        TUint32 color = aBitmap->ReadPixel(x, y);
  //        color ^= 0xffffffff;
  //        aBitmap->PlotPixel(color, x, y);
  //      }
  //    }
  //  }
}

void Display::MoveCursor(TInt aX, TInt aY) {
  if (mMouseX == aX && mMouseY == aY) {
    return;
  }
  // erase old position
  if (!mMouseHidden) {
    render_cursor(mBitmap, mMouseX, mMouseY);
  }

  mMouseX = aX;
  mMouseY = aY;

  // render at new position
  if (!mMouseHidden) {
    render_cursor(mBitmap, mMouseX, mMouseY);
  }
}

TBool Display::ShowCursor() {
  TBool ret = mMouseHidden;
  if (!mMouseHidden) {
    return ret;
  }
  mMouseHidden = EFalse;
  render_cursor(mBitmap, mMouseX, mMouseY);
  return ret;
}

TBool Display::HideCursor() {
  TBool ret = mMouseHidden;
  if (mMouseHidden) {
    return ret;
  }
  mMouseHidden = ETrue;
  render_cursor(mBitmap, mMouseX, mMouseY);
  return ret;
}

void Display::MoveTo(int aX, int aY) {
  mX = aX;
  mY = aY;
}

void Display::GetXY(TInt &aX, TInt &aY) {
  aX = mX;
  aY = mY;
}

void Display::ClearEOL(TUint8 aCharacter) {
  //
}

void Display::Down() {
  //
}

void Display::ScrollUp() {
  //
}

void Display::NewLine() {
  //
}

void Display::WriteChar(char c) {
  //
}

void Display::Clear(TUint32 aColor) {
  mBitmap->Clear(aColor);
  //
}

void Display::WriteString(TInt aX, TInt aY, const char *s) {
  TBool hidden = HideCursor();
  mBitmap->DrawText(aX, aY, s);
  SetCursor(!hidden);
}

void Display::WriteString(const char *s) {
  if (!mMouseHidden) {
    HideCursor();
    mBitmap->DrawText(mX, mY, s);
    ShowCursor();
  }
  else {
    mBitmap->DrawText(mX, mY, s);
  }
}
