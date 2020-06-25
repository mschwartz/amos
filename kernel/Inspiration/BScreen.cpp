#include <Inspiration/Inspiration.h>
#include <Inspiration/BScreen.h>
#include <Exec/ExecBase.h>

#include <Graphics/bitmap/BBitmap32.h>

BScreen::BScreen(const char *aTitle)
    : BNode(aTitle),
      mInspirationBase(*gExecBase.GetInspirationBase()),
      mDisplay(mInspirationBase.GetDisplay()) {

  mBitmap = new BBitmap32(mDisplay.Width(), mDisplay.Height());
  mDirty = EFalse;
  mTopY = 0;
  Clear(0x4f4fff);
}

BScreen::~BScreen() {
  // remove and destroy windows ?
  delete[] mBitmap;
}

void BScreen::AddDirtyRect(TInt32 aX1, TInt32 aY1, TInt32 aX2, TInt32 aY2) {
  TRect rect(aX1, aY1, aX2, aY2);
  mDirtyRects.Add(rect);
}

void BScreen::Clear(const TUint32 aColor) {
  mBitmap->Clear(aColor);
  mDirty = ETrue;
  AddDirtyRect(0, 0, Width() - 1, Height() - 1);
}
