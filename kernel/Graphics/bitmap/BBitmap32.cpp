#include <Exec/ExecBase.h>
#include <Graphics/bitmap/BBitmap32.h>
#include <Graphics/font/BConsoleFont.h>

//BBitmap *BBitmap::CreateBitmap(TInt aWidth, TInt aHeight, TInt aDepth, TInt aPitch, TAny *aMemory) {
//  if (aDepth == 32 || aDepth == 24) {
//    return new BBitmap32(aWidth, aHeight, aPitch, aMemory);
//  }
//  return ENull;
//}

BBitmap32::BBitmap32(TInt aWidth, TInt aHeight, TInt aPitch, TAny *aMemory) {
  mWidth = aWidth;
  mHeight = aHeight;
  mDepth = 32;
  mPitch = aPitch;
  if (mPitch == 0) {
    mPitch = mWidth;
  }

  mFont = ENull;
  if (aMemory) {
    mPixels = (TUint32 *)aMemory;
    mFreePixels = EFalse;
    mPitch /= 4;
  }
  else {
    mPixels = (TUint32 *)AllocMem(aWidth * aHeight * sizeof(TUint32));
    mFreePixels = ETrue;
  }
  mRect.Set(0, 0, mWidth - 1, mHeight - 1);
}

BBitmap32::~BBitmap32() {
  if (mFreePixels && mFreePixels) {
    delete[] mPixels;
    mPixels = nullptr;
  }
  mFreePixels = false;
}

void BBitmap32::Clear(TUint32 aColor) {
  DISABLE;
  for (TInt y = 0; y < mHeight; y++) {
    for (TInt x = 0; x < mWidth; x++) {
      PlotPixel(aColor, x, y);
    }
  }
  ENABLE;
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

void BBitmap32::BltBitmap(BBitmap32 *aOther, TInt aDestX, TInt aDestY) {
  DISABLE;
  TInt w = aOther->Width(),
       h = aOther->Height();

//  dlog("BltBitmap(%d,%d w:%d h:%d\n", aDestX, aDestY, w, h);
  for (TInt y = 0; y < h; y++) {
    for (TInt x = 0; x < w; x++) {
      TUint32 color = aOther->ReadPixel(x, y);
      SafePlotPixel(color, aDestX + x, aDestY + y);
    }
  }
  ENABLE;
}

void BBitmap32::FastLineHorizontal(TUint32 aColor, TInt aX, TInt aY, TUint aW) {
  TInt xmax = aX + aW - 1;
  for (TInt x = aX; x < xmax; x++) {
    if (mRect.PointInRect(x, aY)) {
      PlotPixel(aColor, x, aY);
    }
  }
}

void BBitmap32::FastLineVertical(TUint32 aColor, TInt aX, TInt aY, TUint aH) {
  //  TInt ymax = aY + aH - 1;
  dprintf("\n\n");
  for (TInt y = 0; y < aH; y++) {
//    dlog("flv %d,%d\n", aX, y + aY);
    //    if (mRect.PointInRect(aX, y + aY)) {
    PlotPixel(aColor, aX, y + aY);
    //    }
  }
  dprintf("\n\n");
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
  DISABLE;
  const TInt width = ABS(aX2 - aX1),
             height = ABS(aY2 - aY1);

  dlog("FillRect(%dx%d)\n", width, height);
  if (width > height) {
    for (TInt h = 0; h < height; h++) {
//      dlog("flh %d, %d, %d\n", aX1, aY1, width);
      FastLineHorizontal(aColor, aX1, aY1++, width);
    }
  }
  else {
    for (TInt w = 0; w < width; w++) {
//      dlog("flv %d, %d, %d\n", aX1, aY1, width);
      FastLineVertical(aColor, aX1++, aY1, height);
    }
  }
  ENABLE;
}

void BBitmap32::DrawText(TInt16 aX, TInt16 aY, const char *aString) {
  if (!mFont) {
    return;
  }
  mFont->SetColors(mForegroundColor, mBackgroundColor);
  mFont->Write(aX, aY, aString);
}

#if 0
void BBitmap32::DrawCircle(TUint32 aColor, TInt aX, TInt aY, TUint aRadius) {
}

void BBitmap32::FillCircle(TUint32 aColor, TInt aX, TInt aY, TUint aRadius) {
}
#endif
