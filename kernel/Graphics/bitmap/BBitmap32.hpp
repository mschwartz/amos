#ifndef EXEC_GRAPHICS_BBITMAP32_H
#define EXEC_GRAPHICS_BBITMAP32_H

#include <Graphics/BBitmap.hpp>
//#include <Graphics/BFont.h>

class BConsoleFont32;

extern "C" void CopyRect(TRgbColor *dst, TRgbColor *src, TCoordinate w, TCoordinate h, TCoordinate d1, TCoordinate d2);

class BBitmap32 : public BBase {
public:
  BBitmap32(TCoordinate aWidth, TCoordinate aHeight, TCoordinate aPitch = 0, TAny *aMemory = nullptr);
  ~BBitmap32();

public:
  TBool PointInRect(TCoordinate aX, TCoordinate aY) { return mRect.PointInRect(aX, aY); }

public:
  inline void INLINE PlotPixel(TRGB &aColor, TCoordinate aX, TCoordinate aY) {
    mPixels[aY * mPitch + aX] = aColor.rgb888();
  }

  inline void INLINE PlotPixel(TRgbColor aColor, TCoordinate aX, TCoordinate aY) {
    mPixels[aY * mPitch + aX] = aColor;
  }

  inline void INLINE SafePlotPixel(TRGB &aColor, TCoordinate aX, TCoordinate aY) {
    if (mRect.PointInRect(aX, aY)) {
      mPixels[aY * mPitch + aX] = aColor.rgb888();
    }
  }

  inline void INLINE SafePlotPixel(TRgbColor aColor, TCoordinate aX, TCoordinate aY) {
    if (mRect.PointInRect(aX, aY)) {
      mPixels[aY * mPitch + aX] = aColor;
    }
  }

  inline TRgbColor INLINE ReadPixel(TCoordinate aX, TCoordinate aY) {
    return mPixels[aY * mPitch + aX];
  }

public:
  void Clear(const TRgbColor aColor);
  void Clear(TRGB &aColor) { return Clear(aColor.rgb888()); }
  void CopyPixels(BBitmap32 *aOther);
  TRgbColor *GetPixels() { return mPixels; }

  // copy bitmap from aOther to screen at aDstX, aDsty
  void BltCopy(BBitmap32 *aOther, TCoordinate aDstX, TCoordinate aDstY);
  // copy rectangle (aSrcX, aSrcY, aWidth, aWidth)from aOther bitmap to screen at aDstX, aDsty
  void BltRect(BBitmap32 *aOther, TCoordinate aDstX, TCoordinate aDstY,
    TCoordinate aSrcX, TCoordinate aSrcY, TCoordinate aWidth, TCoordinate aHeight);

public:
  void FastLineHorizontal(TRgbColor aColor, TCoordinate aX, TCoordinate aY, TCoordinate aW);

  void FastLineVertical(TRgbColor aColor, TCoordinate aX, TCoordinate aY, TCoordinate aH);

  void DrawLine(TRgbColor aColor, TCoordinate aX1, TCoordinate aY1, TCoordinate aX2, TCoordinate aY2);
  void DrawLine(TRGB &aColor, TRect &aRect) {
    DrawLine(aColor.rgb888(), aRect.x1, aRect.y1, aRect.x2, aRect.y2);
  }

  void DrawRect(TRgbColor aColor, TCoordinate aX1, TCoordinate aY1, TCoordinate aX2, TCoordinate aY2);
  void DrawRect(TRGB &aColor, TRect &aRect) {
    DrawRect(aColor.rgb888(), aRect.x1, aRect.y1, aRect.x2, aRect.y2);
  }

  void FillRect(TRgbColor aColor, TCoordinate aX1, TCoordinate aY1, TCoordinate aX2, TCoordinate aY2);
  void FillRect(TRGB &aColor, TRect &aRect) {
    FillRect(aColor.rgb888(), aRect.x1, aRect.y1, aRect.x2, aRect.y2);
  }

#if 0
  void DrawCircle(TRgbColor aColor, TCoordinate aX, TCoordinate aY, TCoordinate r);

  void FillCircle(TRgbColor aColor, TCoordinate aX, TCoordinate aY, TCoordinate r);
#endif

public:
  void SetColors(TRGB &aForegroundColor, TRGB &aBackgroundColor) {
    mForegroundColor.Set(aForegroundColor);
    mBackgroundColor.Set(aBackgroundColor);
  }
  void SetColors(TUint32 aForegroundColor, TUint32 aBackgroundColor) {
    TRGB fg(aForegroundColor),
      bg(aBackgroundColor);
    SetColors(fg, bg);
  }
  void GetColors(TRGB &aForegroundColor, TRGB &aBackgroundColor) {
    aForegroundColor.Set(mForegroundColor);
    aBackgroundColor.Set(mBackgroundColor);
  }
  void SetFont(BConsoleFont32 *aFont) { mFont = aFont; }
  void DrawText(TCoordinate aX, TCoordinate aY, const char *aString);

public:
  void GetRect(TRect &aRect) {
    aRect.x1 = mRect.x1;
    aRect.y1 = mRect.y1;
    aRect.x2 = mRect.x2;
    aRect.y2 = mRect.y2;
  }

  TCoordinate Width() { return mWidth; }
  TCoordinate Height() { return mHeight; }
  TCoordinate Depth() { return mDepth; }

public:
  void Dump() {
    dlog("BBitmap32 at %x\n", this);
    dlog("   width: %d, height: %d, depth: %d, pitch: %d\n", mWidth, mHeight, mDepth, mPitch);
    dlog("   mFont: %x mPixels: %x\n", mFont, mPixels);
    dlog("   mRect\n");
    mRect.Dump();
    dprint("\n\n");
  }

protected:
  TCoordinate mWidth, mHeight, mDepth, mPitch;
  BConsoleFont32 *mFont;
  TRgbColor *mPixels;
  TRGB mForegroundColor, mBackgroundColor;
  TBool mFreePixels;
  TRect mRect;
};

#endif
