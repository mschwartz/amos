#include "BBitmap32.h"

BBitmap32::BBitmap32(TInt aWidth, TInt aHeight, TInt aDepth, TAny *aMemory)
    : BBitmap(aWidth, aHeight, aDepth, aMemory) {
  if (aMemory != ENull) {
    mPixels32 = (TUint32 *)aMemory;
  }
  else {
    mPixels32 = (TUint32 *)AllocMem(aWidth * aHeight, MEMF_FAST);
  }
  mPixels = mPixels32;
}

BBitmap32::~BBitmap32() {
  if (mFreePixels && mPixels32) {
    delete[] mPixels32;
    mPixels = mPixels32 = nullptr;
  }
  mFreePixels = false;
}

void BBitmap32::Clear(TRGB &aColor) {
  TUint32 color = aColor.rgb888();
  TUint32 *pix = mPixels32;
  for (TInt p = 0; p < mHeight * mWidth; p++) {
    *pix++ = color;
  }
}

void BBitmap32::Line(TFloat aX1, TFloat aY1, TFloat aX2, TFloat aY2, TRGB &aColor) {
}

void BBitmap32::FastHLine(TInt aX, TInt aY, TUint aW, TRGB &aColor) {
}

void BBitmap32::FastVLine(TInt aX, TInt aY, TUint aH, TRGB &aColor) {
}

void DrawRect(TInt aX1, TInt aY1, TInt aX2, TInt aY2, TRGB& aColor){
  //
}
void DrawRect(TRect& aRect, TRGB& aColor){
  //
}
void FillRect(TInt aX1, TInt aY1, TInt aX2, TInt aY2, TRGB& aColor){
  //
}
void FillRect(TRect& aRect, TRGB& aColor){
  //
}

void DrawBitmap(BBitmap *srcBitmap, TRect& aSrcRect, TInt aDstX, TInt aDstY, TUint32 aFlags = 0) {
  //
}
void DrawBitmapTransparent(BBitmap *srcBitmap, TRect& aSrcRect, TInt aDstX, TInt aDstY, TUint32 aFlags = 0) {
  //
}

void DrawString(const char *aStr, const BFont *aFont, TInt aDx, TInt aDy, TRGB& aFgColor, TRGB& aBgColor) {
  //
}

