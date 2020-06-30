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
    // dlog("Allocating pixels (%d)\n", aWidth * aHeight * sizeof(TUint32));
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

static void CopyPixels32(TUint32 *dst, TUint32 *src, TUint32 len) {
  // rsi = e000 0000 (dst)
  // rdx = src
  // rcx = len
  dlog("dst(%x) src(%x) len(%d)\n", dst, src, len);
}

extern "C" void CopyRect(TUint32 *dst, TUint32 *src, TUint32 w, TUint32 h, TUint64 d1, TUint64 d2);

void BBitmap32::BltBitmap(BBitmap32 *aOther, TInt aDestX, TInt aDestY) {
  TInt w = aOther->Width(),
       h = aOther->Height();

  if (aDestX + w > mWidth) {
    w = mWidth - aDestX;
  }
  if (aDestY + h > mHeight) {
    h = mHeight - aDestY;
  }

  TUint32 *src = &aOther->mPixels[0],
          *dst = &mPixels[aDestY * mPitch + aDestX];

  CopyRect(dst, src, w, h, mPitch * 4, aOther->mPitch * 4);
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

void BBitmap32::DrawText(TInt16 aX, TInt16 aY, const char *aString) {
  if (!mFont) {
    return;
  }
  mFont->SetColors(mForegroundColor, mBackgroundColor);
  mFont->Write(this, aX, aY, aString);
}

#if 0
void BBitmap32::DrawCircle(TUint32 aColor, TInt aX, TInt aY, TUint aRadius) {
}

void BBitmap32::FillCircle(TUint32 aColor, TInt aX, TInt aY, TUint aRadius) {
}
#endif
