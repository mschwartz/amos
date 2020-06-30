#include <Inspiration/InspirationBase.h>
#include <Inspiration/BScreen.h>
#include <Exec/ExecBase.h>

#include <Graphics/bitmap/BBitmap32.h>

BScreen::BScreen(const char *aTitle) : BNode(aTitle), mInspirationBase(*gExecBase.GetInspirationBase()) {
  mDisplay = mInspirationBase.GetDisplay();
  dlog("mDisplay(%x)\n", mDisplay);
  mDisplay->Dump();
  mBitmap = new BBitmap32(mDisplay->Width(), mDisplay->Height());
  mDirty = EFalse;
  mTopY = 0;

  // Clear(0x4f4fff);
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
  dlog("AddDirtyRect\n");
  rect.Dump();
  mDirtyRects.Add(rect);
}

void BScreen::Clear(const TUint32 aColor) {
  mBitmap->Clear(aColor);
  mDirty = ETrue;
  AddDirtyRect(0, 0, Width() - 1, Height() - 1);
  UpdateDirtyRects();
}

void BScreen::UpdateWindow(BWindow *aWindow, TBool aDecorations) {
  TRect& rect = aWindow->mWindowRect;
  mBitmap->BltBitmap(aWindow->mBitmap, rect.x1, rect.y1);
  AddDirtyRect(rect.x1, rect.y1, rect.x2, rect.y2);
  UpdateDirtyRects();
}

void BScreen::UpdateDirtyRects() {
  for (DirtyRect *r = mDirtyRects.First(); !mDirtyRects.End(r); r = mDirtyRects.Next(r)) {
    r->Remove();
    TRect& rect = r->mRect;
    BBitmap32 *b = mDisplay->GetBitmap();
    for (TInt y = rect.y1; y <= rect.y2; y++) {
      for (TInt x = rect.x1; x <= rect.x2; x++) {
	TUint32 color = mBitmap->ReadPixel(x,y);
	b->PlotPixel(color, x, y);
      }
    }
    delete r;
  }
}
