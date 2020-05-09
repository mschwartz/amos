#ifndef EXEC_GRAPHICS_BBITMAP32_H
#define EXEC_GRAPHICS_BBITMAP32_H

#include <Graphics/BBitmap.h>
//#include <Graphics/BFont.h>

class BConsoleFont32;

class BBitmap32 : public BBase {
public:
  BBitmap32(TInt32 aWidth, TInt32 aHeight, TInt32 aPitch, TAny *aMemory = nullptr);
  ~BBitmap32();

public:
  TBool PointInRect(TInt aX, TInt aY) { return mRect.PointInRect(aX, aY); }

public:
  inline void INLINE PlotPixel(TRGB &aColor, TInt32 aX, TInt32 aY) {
    mPixels[aY * mPitch + aX] = aColor.rgb888();
  }

  inline void INLINE PlotPixel(TUint32 aColor, TInt32 aX, TInt32 aY) {
    mPixels[aY * mPitch + aX] = aColor;
  }

  inline void INLINE SafePlotPixel(TRGB &aColor, TInt32 aX, TInt32 aY) {
    if (mRect.PointInRect(aX, aY)) {
      mPixels[aY * mPitch + aX] = aColor.rgb888();
    }
  }

  inline void INLINE SafePlotPixel(TUint32 aColor, TInt32 aX, TInt32 aY) {
    if (mRect.PointInRect(aX, aY)) {
      mPixels[aY * mPitch + aX] = aColor;
    }
  }

  inline TUint32 INLINE ReadPixel(TInt32 aX, TInt32 aY) {
    return mPixels[aY * mPitch + aX];
  }

public:
  void Clear(const TUint32 aColor);
  void Clear(TRGB &aColor) { return Clear(aColor.rgb888()); }
  void CopyPixels(BBitmap32 *aOther);

public:
  void FastLineHorizontal(TUint32 aColor, TInt aX, TInt aY, TUint aW);

  void FastLineVertical(TUint32 aColor, TInt aX, TInt aY, TUint aH);

  void DrawLine(TUint32 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2);
  void DrawLine(TRGB &aColor, TRect &aRect) {
    DrawLine(aColor.rgb888(), aRect.x1, aRect.y1, aRect.x2, aRect.y2);
  }

  void DrawRect(TUint32 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2);
  void DrawRect(TRGB &aColor, TRect &aRect) {
    DrawRect(aColor.rgb888(), aRect.x1, aRect.y1, aRect.x2, aRect.y2);
  }

  void FillRect(TUint32 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2);
  void FillRect(TRGB &aColor, TRect &aRect) {
    FillRect(aColor.rgb888(), aRect.x1, aRect.y1, aRect.x2, aRect.y2);
  }

#if 0
  void DrawCircle(TUint32 aColor, TInt aX, TInt aY, TUint r);

  void FillCircle(TUint32 aColor, TInt aX, TInt aY, TUint r);
#endif

public:
  void SetColors(TRGB &aForegroundColor, TRGB &aBackgroundColor) {
    mForegroundColor.Set(aForegroundColor);
    mBackgroundColor.Set(aBackgroundColor);
  }
  void GetColors(TRGB &aForegroundColor, TRGB &aBackgroundColor) {
    aForegroundColor.Set(mForegroundColor);
    aBackgroundColor.Set(mBackgroundColor);
  }
  void SetFont(BConsoleFont32 *aFont) { mFont = aFont; }
  void DrawText(TInt16 aX, TInt16 aY, const char *aString);

public:
  void GetRect(TRect &aRect) {
    aRect.x1 = mRect.x1;
    aRect.y1 = mRect.y1;
    aRect.x2 = mRect.x2;
    aRect.y2 = mRect.y2;
  }
  TInt Width() { return mWidth; }
  TInt Height() { return mHeight; }

public:
  void Dump() {
    dprint("BBitmap32 at %x\n", this);
    dprint("   width: %d, height: %d, depth: %d, pitch: %d\n", mWidth, mHeight, mDepth, mPitch);
    dprint("   mFont: %x mPixels: %x\n", mFont, mPixels);
    mRect.Dump();
    dprint("\n\n");
  }

protected:
  TInt mWidth, mHeight, mDepth, mPitch;
  BConsoleFont32 *mFont;
  TUint32 *mPixels;
  TRGB mForegroundColor, mBackgroundColor;
  TBool mFreePixels;
  TRect mRect;
};

#endif
