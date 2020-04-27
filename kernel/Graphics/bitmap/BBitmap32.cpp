#include <Graphics/bitmap/BBitmap32.h>

//BBitmap *BBitmap::CreateBitmap(TInt aWidth, TInt aHeight, TInt aDepth, TInt aPitch, TAny *aMemory) {
//  if (aDepth == 32 || aDepth == 24) {
//    return new BBitmap32(aWidth, aHeight, aPitch, aMemory);
//  }
//  return ENull;
//}

BBitmap32::BBitmap32(TInt aWidth, TInt aHeight, TInt aPitch, TAny *aMemory)
    : BBitmap(aWidth, aHeight, 32, aPitch, aMemory) {
  if (aMemory) {
    mPixels = (TUint32 *)aMemory;
  }
  else {
    mPixels = (TUint32 *)AllocMem(aWidth * aHeight);
  }
}

BBitmap32::~BBitmap32() {
  if (mFreePixels && mFreePixels) {
    delete[] mPixels;
    mPixels = nullptr;
  }
  mFreePixels = false;
}

void BBitmap32::ClearScreen(TUint32 aColor) {
  for (TInt y = 0; y < mHeight; y++) {
    for (TInt x = 0; x < mWidth; x++) {
      PlotPixel(aColor, x, y);
    }
  }
}

void BBitmap32::Clear(TUint32 aColor) {
  for (TInt y = 0; y < mHeight; y++) {
    for (TInt x = 0; x < mWidth; x++) {
      PlotPixel(aColor, x, y);
    }
  }
}

void BBitmap32::CopyPixels(BBitmap32 *aOther) {
  TInt32 w = MIN(Width(), aOther->Width()),
         h = MIN(Height(), aOther->Height());

  for (TInt32 y = 0; y < h; y++) {
    for (TInt32 x = 0; x < h; x++) {
      TUint32 color = aOther->ReadPixel(x, y);
    }
  }
}

void BBitmap32::FastLineHorizontal(TUint32 aColor, TInt aX, TInt aY, TUint aW) {
  TInt xmax = aX + aW - 1;
//  dlog("flh %d,%d %d/%d\n", aX, aY, aW, xmax);
  for (TInt x = aX; x < xmax; x++) {
    if (mRect.PointInRect(x, aY)) {
//      dlog("flh x=%d/%d\n", x, xmax);
      PlotPixel(aColor, x, aY);
    }
//    dlog("XXX\n");
  }
}

void BBitmap32::FastLineVertical(TUint32 aColor, TInt aX, TInt aY, TUint aH) {
  TInt ymax = aY + aH - 1;
//  dlog("flv %d,%d %d/%d\n", aX, aY, aH, ymax);
  for (volatile TInt y = aY; y < ymax; y++) {
    if (mRect.PointInRect(aX, y)) {
//      dlog("flv y=%d/%d\n", y, ymax);
      PlotPixel(aColor, aX, y);
    }
//    dlog("XXX\n");
  }
}

void BBitmap32::DrawLine(TUint32 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2) {
  TInt dx, dy, p, x, y;
  dx = aX2 - aX1;
  dy = aY2 - aY1;

  x = aX1;
  y = aY1;

  p = 2 * dy - dx;
  while (x < aX2) {
    PlotPixel(aColor, x, y);
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

void BBitmap32::DrawRect(TUint32 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2) {
  const TInt width = aX2 - aX1 + 1;
  const TInt height = aY2 - aY1 + 1;
  FastLineHorizontal(aColor, aX1, aY1, width);
  FastLineHorizontal(aColor, aX1, aY2, width);
  FastLineVertical(aColor, aX1, aY1, height);
  FastLineVertical(aColor, aX2, aY1, height);
}

void BBitmap32::FillRect(TUint32 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2) {
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

#if 0
void BBitmap32::DrawCircle(TUint32 aColor, TInt aX, TInt aY, TUint aRadius) {
}

void BBitmap32::FillCircle(TUint32 aColor, TInt aX, TInt aY, TUint aRadius) {
}
#endif

