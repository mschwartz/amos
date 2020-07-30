#include <Exec/ExecBase.hpp>
#include <Graphics/Graphics.hpp>
#include <Graphics/bitmap/BBitmap32.hpp>
#include <Graphics/font/BConsoleFont.hpp>

//BBitmap *BBitmap::CreateBitmap(TInt aWidth, TInt aHeight, TInt aDepth, TInt aPitch, TAny *aMemory) {
//  if (aDepth == 32 || aDepth == 24) {
//    return new BBitmap32(aWidth, aHeight, aPitch, aMemory);
//  }
//  return ENull;
//}

BBitmap32::BBitmap32(TCoordinate aWidth, TCoordinate aHeight, TCoordinate aPitch, TAny *aMemory) {
  mWidth = aWidth;
  mHeight = aHeight;
  mDepth = 32;
  mPitch = aPitch;
  if (mPitch == 0) {
    mPitch = mWidth;
  }

  mFont = ENull;
  if (aMemory) {
    mPixels = (TRgbColor *)aMemory;
    mFreePixels = EFalse;
    mPitch /= 4;
  }
  else {
    // dlog("Allocating pixels (%d)\n", aWidth * aHeight * sizeof(TRgbColor));
    mPixels = (TRgbColor *)AllocMem(aWidth * aHeight * sizeof(TRgbColor));
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

void BBitmap32::Clear(TRgbColor aColor) {
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
      TRgbColor color = aOther->ReadPixel(x, y);
    }
  }
}

void BBitmap32::BltRect(BBitmap32 *aOther, TCoordinate aDstX, TCoordinate aDstY, TCoordinate aSrcX, TCoordinate aSrxY, TCoordinate aSrcWidth, TCoordinate aSrcHeight) {
  TRgbColor *dst = &mPixels[aDstY * mPitch + aDstX],
            *src = &aOther->mPixels[aSrxY * aOther->mPitch + aSrcX];

  TCoordinate dd = mPitch,
              ds = aOther->mPitch,
              width = aSrcWidth,
              height = aSrcHeight;

  if (width & 1) {
    for (TCoordinate i = 0; i < height; i++) {
      CopyRGB(dst, src, width);
      dst += dd;
      src += ds;
    }
  }
  else {
    width /= 2;
    for (TCoordinate i = 0; i < height; i++) {
      CopyRGB64(dst, src, width);
      dst += dd;
      src += ds;
    }
  }
}

void BBitmap32::BltCopy(BBitmap32 *aOther, TCoordinate aDstX, TCoordinate aDstY) {
  TRgbColor *dst = &mPixels[aDstY * mWidth + aDstX],
            *src = &aOther->mPixels[0];

  TCoordinate dd = mPitch,
              ds = aOther->mPitch,
              width = aOther->mWidth,
              height = aOther->mHeight;

  if (width & 1) {
    // odd, do 1 pixel at a time
    for (TCoordinate y = 0; y < height; y++) {
      CopyRGB(dst, src, width);
      dst += dd;
      src += ds;
    }
  }
  else {
    // even, do 2 pixels at a time
    width /= 2;
    for (TCoordinate y = 0; y < height; y++) {
      CopyRGB64(dst, src, width);
      dst += dd;
      src += ds;
    }
  }
}

void BBitmap32::FastLineHorizontal(TRgbColor aColor, TCoordinate aX, TCoordinate aY, TCoordinate aW) {
  TInt xmax = aX + aW - 1;
  for (TInt x = aX; x < xmax; x++) {
    if (mRect.PointInRect(x, aY)) {
      PlotPixel(aColor, x, aY);
    }
  }
}

void BBitmap32::FastLineVertical(TRgbColor aColor, TCoordinate aX, TCoordinate aY, TCoordinate aH) {
  //  TInt ymax = aY + aH - 1;
  dprintf("\n\n");
  for (TInt y = 0; y < aH; y++) {
    //    if (mRect.PointInRect(aX, y + aY)) {
    PlotPixel(aColor, aX, y + aY);
    //    }
  }
  dprintf("\n\n");
}

void BBitmap32::DrawLine(TRgbColor aColor, TCoordinate aX1, TCoordinate aY1, TCoordinate aX2, TCoordinate aY2) {
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

void BBitmap32::DrawRect(TRgbColor aColor, TCoordinate aX1, TCoordinate aY1, TCoordinate aX2, TCoordinate aY2) {
  const TInt width = aX2 - aX1 + 1;
  const TInt height = aY2 - aY1 + 1;
  FastLineHorizontal(aColor, aX1, aY1, width);
  FastLineHorizontal(aColor, aX1, aY2, width);
  FastLineVertical(aColor, aX1, aY1, height);
  FastLineVertical(aColor, aX2, aY1, height);
}

void BBitmap32::FillRect(TRgbColor aColor, TCoordinate aX1, TCoordinate aY1, TCoordinate aX2, TCoordinate aY2) {
  const TInt width = ABS(aX2 - aX1),
             height = ABS(aY2 - aY1);

  if (width > height) {
    for (TInt h = 0; h < height; h++) {
      FastLineHorizontal(aColor, aX1, aY1++, width);
    }
  }
  else {
    for (TInt w = 0; w < width; w++) {
      FastLineVertical(aColor, aX1++, aY1, height);
    }
  }
}

void BBitmap32::DrawText(TCoordinate aX, TCoordinate aY, const char *aString) {
  if (!mFont) {
    return;
  }
  mFont->SetColors(mForegroundColor, mBackgroundColor);
  mFont->Write(this, aX, aY, aString);
}

#if 0
void BBitmap32::DrawCircle(TRgbColor aColor, TInt aX, TInt aY, TUint aRadius) {
}

void BBitmap32::FillCircle(TRgbColor aColor, TInt aX, TInt aY, TUint aRadius) {
}
#endif
