#ifndef EXEC_GRAPHICS_BBITMAP32_H
#define EXEC_GRAPHICS_BBITMAP32_H

#include <Graphics/BBitmap.h>

class BBitmap32 : public BBitmap {
public:
  BBitmap32(TInt32 aWidth, TInt32 aHeight, TInt32 aPitch, TAny *aMemory = nullptr);
  ~BBitmap32();

public:
  inline void INLINE PlotPixel(TRGB aColor, TInt32 aX, TInt32 aY) {
    mPixels[aY * mPitch + aX] = aColor.rgb888();
  }

  inline void INLINE PlotPixel(TUint32 aColor, TInt32 aX, TInt32 aY) {
    mPixels[aY * mPitch + aX] = aColor;
  }

  inline void INLINE SafePlotPixel(TRGB aColor, TInt32 aX, TInt32 aY) {
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

  void ClearScreen(TUint32 aColor);

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

protected:
  TUint32 *mPixels;
};

#endif
