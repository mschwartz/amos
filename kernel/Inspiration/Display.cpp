#include <Exec/ExecBase.h>
#include <Inspiration/Display.h>
#include <Inspiration/BScreen.h>

void Display::AddScreen(BScreen *aScreen) {
  mScreenList->AddHead(*aScreen);
  aScreen->Clear(0x4f4fff);
}

BScreen *Display::Find(const char *aTitle) {
  return mScreenList->Find(aTitle);
}

Display::Display() : BNode("Display") {
  dlog("Construct Display\n");
  mScreenList = new BScreenList;
  TSystemInfo info;
  gExecBase.GetSystemInfo(&info);

  TUint64 fb = (TUint64)info.mDisplayFrameBuffer;
  mBitmap = new BBitmap32(info.mDisplayWidth, info.mDisplayHeight, info.mDisplayPitch, (TAny *)fb);
  // mBitmap->Dump();

  mMouseX = mMouseY = -1;
  mMouseHidden = ETrue;
  MoveCursor(20, 20);
  ShowCursor();
}

static const char *cursor_image[] = {
  "Booooooooooo",
  "BBoooooooooo",
  "BXBooooooooo",
  "BXBBoooooooo",
  "BXXXBooooooo",
  "BXXXXBoooooo",
  "BXXXXXBooooo",
  "BXXXXXXBoooo",
  "BXXXXXXXBooo",
  "BXXXXXXXXBoo",
  "BXXXXXXXXXBo",
  "BXXXXXXBBBBB",
  "BXXXBXXBoooo",
  "BXXBBXXBoooo",
  "BXBooBXXBooo",
  "BBoooBXXBooo",
  "BoooooBXXBoo",
  "ooooooBXXBoo",
  "oooooooBXXBo",
  "oooooooBXXBo",
  "ooooooooBBoo",
};

static const TInt cursor_width = 12, cursor_height = 21;

#if 1
static void render_cursor(BBitmap32 *aBitmap, TInt aX, TInt aY) {
  dlog("render_cursor(%x, %d, %d)", aBitmap, aX, aY);
  for (TInt y = 0; y < cursor_height; y++) {
    const char *src = cursor_image[y];
    for (TInt x = 0; x < cursor_width; x++) {
      switch (*src++) {
        case 'B':
          // render pixel below half bright
          {
            TRGB c(aBitmap->ReadPixel(aX + x, aY + y));
            c.r /= 2;
            c.g /= 2;
            c.b /= 2;
            aBitmap->PlotPixel(c, aX + x, aY + y);
          }
          break;
        case 'X':
          // render black pixel
          {
            aBitmap->PlotPixel(0x000000, aX + x, aY + y);
          }
          break;
        default:
          break;
      }
    }
  }
}

#else
// #include <Inspiration/cursors/pointer.h>
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
}
#endif

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

  void Display::GetXY(TInt & aX, TInt & aY) {
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
