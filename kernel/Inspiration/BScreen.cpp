#include <Inspiration/InspirationBase.h>
#include <Exec/ExecBase.h>
#include <Inspiration/BScreen.h>
#include <Inspiration/Display.h>
#include <Inspiration/Display/Cursor.h>

#include <Graphics/bitmap/BBitmap32.h>

BScreen::BScreen(const char *aTitle) : BNode(aTitle), mInspirationBase(*gExecBase.GetInspirationBase()) {
  mDisplay = mInspirationBase.GetDisplay();
  mDisplay->Dump();
  mBitmap = new BBitmap32(mDisplay->Width(), mDisplay->Height());
  mDirty = EFalse;
  mTopY = 0;
}

BScreen::~BScreen() {
  // remove and destroy windows ?
  delete[] mBitmap;
}

void BScreen::AddWindow(BWindow *aWindow) {
  aWindow->mScreen = this;
  mWindowList.AddHead(*aWindow);
  aWindow->PaintDecorations();
}

extern "C" TUint64 GetRSP();

void BScreen::AddDirtyRect(TInt32 aX1, TInt32 aY1, TInt32 aX2, TInt32 aY2) {
  TRect rect(aX1, aY1, aX2, aY2);
  mDirtyRects.Add(rect);
}

void BScreen::Clear(const TUint32 aColor) {
  mBitmap->Clear(aColor);
  mDirty = ETrue;
  AddDirtyRect(0, 0, Width() - 1, Height() - 1);
}

void BScreen::UpdateWindow(BWindow *aWindow, TBool aDecorations) {
  TRect &rect = aWindow->mWindowRect;
  mBitmap->BltBitmap(aWindow->mBitmap, rect.x1, rect.y1);
  AddDirtyRect(rect.x1, rect.y1, rect.x2, rect.y2);
}

// void BScreen::RenderCursor(Cursor *aCursor, TInt32 aX, TInt32 aY) {
//   aCursor->Render(mBitmap, aX, aY);
//   AddDirtyRect(aX, aY, aX + aCursor->Width() - 1, aY + aCursor->Height() - 1);
// }

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
