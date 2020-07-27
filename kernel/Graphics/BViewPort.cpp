#include <Graphics/BViewPort.hpp>
#include <Graphics/BFont.hpp>
#include <Graphics/BBitmap.hpp>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

// constructor
BViewPort::BViewPort(const char *aName, BBitmap *aBitmap) : BNodePri(aName) {
  //  dlog("BViewPort constructor\n");
  mBitmap = aBitmap;
}

BViewPort::~BViewPort() {
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

// constructor
BViewPort32::BViewPort32(const char *aName, BBitmap32 *aBitmap) : BViewPort(aName, (BBitmap *)aBitmap) {
  mBitmap32 = aBitmap;
  mBitmap32->GetRect(mRect);
}

BViewPort32::~BViewPort32() {
}

void BViewPort32::Clear(TUint32 aColor) {
  TRGB c(aColor);
  mBitmap32->FillRect(c, mRect);
}

void BViewPort32::FastLineHorizontal(TUint32 aColor, TInt aX, TInt aY, TUint aW) {
  TInt x1 = mRect.x1 + aX,
       y1 = mRect.y1 + aY;
  TInt xmax = x1 + +aW - 1;

  for (TInt x = x1; x < xmax; x++) {
    if (mRect.PointInRect(x, y1)) {
      mBitmap32->PlotPixel(aColor, x, y1);
    }
  }
}

void BViewPort32::FastLineVertical(TUint32 aColor, TInt aX, TInt aY, TUint aH) {
  TInt x1 = mRect.x1 + aX,
       y1 = mRect.y1 + aY,
       ymax = mRect.y1 + aY + aH - 1;

  // mRect.Dump();
  // dlog("x1(%d) y1(%d) ymax(%d)\n", x1, y1, ymax);
  for (TInt y = y1; y < ymax; y++) {
    if (mRect.PointInRect(x1, y)) {
      mBitmap32->PlotPixel(aColor, x1, y);
    }
    // else {
    //   dlog("CLIPPED\n");
    // }
  }
}

void BViewPort32::DrawLine(TUint32 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2) {
  TInt x1 = mRect.x1,
       y1 = mRect.y1;

  TInt dx, dy, p, x, y;
  dx = aX2 - aX1;
  dy = aY2 - aY1;

  x = aX1;
  y = aY1;

  p = 2 * dy - dx;
  while (x < aX2) {
    mBitmap32->SafePlotPixel(aColor, x + x1, y + 11);
    if (p == 0) {
      y++;
      p = p + 2 * dy - 2 * dx;
    }
    else {
      p = p + 2 * dy;
    }
    x++;
  }
}

void BViewPort32::DrawRect(TUint32 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2) {
  const TInt width = aX2 - aX1 + 1;
  const TInt height = aY2 - aY1 + 1;
  FastLineHorizontal(aColor, aX1, aY1, width);
  FastLineHorizontal(aColor, aX1, aY2, width);
  FastLineVertical(aColor, aX1, aY1, height);
  FastLineVertical(aColor, aX2, aY1, height);
}

void BViewPort32::FillRect(TUint32 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2) {
  const TInt width = ABS(aX2 - aX1 + 1);
  const TInt height = ABS(aY2 - aY1 + 1);

  if (width > height) {
    TInt h = height;
    while (h--) {
      FastLineHorizontal(aColor, aX1, aY1++, width);
    }
  }
  else {
    TInt w = width;
    while (w--) {
      FastLineVertical(aColor, aX1++, aY1, height);
    }
  }
}

void BViewPort32::DrawText(TInt16 aX, TInt16 aY, const char aChar) {
  if (!mFont) {
    dlog("*** DrawText, no font\n");
    return;
  }
  mFont->SetColors(mForegroundColor, mBackgroundColor);
  mFont->Write(mBitmap32, mRect.x1 + aX, mRect.y1 + aY, aChar);
}

void BViewPort32::DrawText(TInt16 aX, TInt16 aY, const char *aString) {
  if (!mFont) {
    dlog("*** DrawText(%d,%d) aString(%s), no font\n", aX, aY, aString);
    return;
  }
  mFont->SetColors(mForegroundColor, mBackgroundColor);
  mFont->Write(mBitmap32, mRect.x1 + aX, mRect.y1 + aY, aString);
}

void BViewPort32::DrawTextTransparent(TInt16 aX, TInt16 aY, const char *aString) {
  if (!mFont) {
    dlog("*** DrawText, no font\n");
    return;
  }
  mFont->SetColors(mForegroundColor, mBackgroundColor);
  mFont->WriteTransparent(mBitmap32, mRect.x1 + aX, mRect.y1 + aY, aString);
}

void BViewPort32::DrawTextTransparent(TInt16 aX, TInt16 aY, TUint32 aColor, const char *aString) {
  if (!mFont) {
    dlog("*** DrawText, no font\n");
    return;
  }
  TRGB c(aColor);
  mFont->SetColors(c, mBackgroundColor);
  mFont->WriteTransparent(mBitmap32, mRect.x1 + aX, mRect.y1 + aY, aString);
}
