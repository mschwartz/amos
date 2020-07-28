#include <Inspiration/InspirationBase.hpp>
#include <Exec/ExecBase.hpp>
#include <Inspiration/BScreen.hpp>
#include <Inspiration/Display.hpp>
#include <Inspiration/Cursor.hpp>

#include <Graphics/bitmap/BBitmap32.hpp>

BScreen::BScreen(const char *aTitle) : BNode(aTitle), mInspirationBase(*gExecBase.GetInspirationBase()) {
  mDisplay = mInspirationBase.GetDisplay();
  mDisplay->Dump();
  mBitmap = new BBitmap32(mDisplay->Width(), mDisplay->Height());
  mTopY = 0;
  mTheme = new BTheme("Default Theme");
  mTheme->Dump();
}

BScreen::~BScreen() {
  // remove and destroy windows ?
  delete[] mBitmap;
}

void BScreen::AddWindow(BWindow *aWindow) {
  aWindow->mScreen = this;
  aWindow->mNext = aWindow->mPrev = ENull;
  ActivateWindow(aWindow);
}

extern "C" TUint64 GetRSP();

void BScreen::AddDirtyRect(TInt32 aX1, TInt32 aY1, TInt32 aX2, TInt32 aY2) {
  TRect rect(aX1, aY1, aX2, aY2);
  mDirtyRects.Add(rect);
}

void BScreen::Clear(const TUint32 aColor) {
  mBitmap->Clear(aColor);
  AddDirtyRect(0, 0, Width() - 1, Height() - 1);
}

void BScreen::UpdateWindow(BWindow *aWindow, TBool aDecorations) {
  TRect &rect = aWindow->mWindowRect;
  mBitmap->BltBitmap(aWindow->mBitmap, rect.x1, rect.y1);
  AddDirtyRect(rect.x1, rect.y1, rect.x2, rect.y2);
}

void BScreen::ActivateWindow(BWindow *aWindow) {
  if (aWindow->mNext != ENull) {
    aWindow->Remove();
  }
  BWindow *w = ActiveWindow();
  mWindowList.AddHead(*aWindow);
  if (!mWindowList.End(w)) {
    w->PaintDecorations(); // inactivate window
  }
  aWindow->PaintDecorations();
}

TBool BScreen::ActivateWindow(TInt32 aX, TInt32 aY) {
  BWindow *selected = ENull;

  DISABLE;
  for (BWindow *w = mWindowList.Last(); !mWindowList.End(w); w = (BWindow *)mWindowList.Prev(w)) {
    // dlog("ActivateWindow, trying %x(%s) %d,%d %d\n", w, w->Title(), aX, aY, w->mWindowRect.PointInRect(aX, aY));
    if (w->mWindowRect.PointInRect(aX, aY)) {
      selected = w;
    }
  }
  ENABLE;
  if (selected) {
    if (selected != ActiveWindow()) {
      ActivateWindow(selected);
      return ETrue;
    }
  }
  else {
    dlog("Click outside all windows\n");
  }

  return EFalse;
}

// This is called from the DisplayTask to render the dirty rects from offscreen to
// physical screen.  Called during vblank to try to avoid tearing.
void BScreen::UpdateDirtyRects() {
  for (DirtyRect *r = mDirtyRects.First(); !mDirtyRects.End(r); r = mDirtyRects.Next(r)) {
    r->Remove();
    TRect &rect = r->mRect;
    // TODO: code in assembly for more speed!
    BBitmap32 *b = mDisplay->GetBitmap();
    for (TInt y = rect.y1; y <= rect.y2; y++) {
      for (TInt x = rect.x1; x <= rect.x2; x++) {
        TUint32 color = mBitmap->ReadPixel(x, y);
        b->PlotPixel(color, x, y);
      }
    }
    delete r;
  }
}

void BScreen::UpdateWindows() {
  // render windows back to front
  for (BWindow *win = mWindowList.Last(); !mWindowList.End((BNode *)win); win = (BWindow *)mWindowList.Prev(win)) {
    TBool hidden = EFalse;
    for (BWindow *other = (BWindow *)mWindowList.First(); other != win; other = (BWindow *)mWindowList.Next(other)) {
      if (win->Obscured(other)) {
        hidden = ETrue;
        break;
      }
    }
    if (!hidden) {
      win->Repaint();
      UpdateWindow(win);
    }
  }
}
