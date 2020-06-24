#ifndef GRAPHICS_BVIEWPORT_H
#define GRAPHICS_BVIEWPORT_H

#include <Exec/Types/BList.h>
#include <Graphics/BBitmap.h>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BBitmap;

class BViewPort : public BNodePri {
public:
  BViewPort(const char *aName, BBitmap *aBitmap);
  ~BViewPort();

public:
  void GetRect(TRect &aRect) { aRect = mRect; }
  TInt Width() { return mRect.Width(); }
  TInt Height() { return mRect.Height(); }

protected:
  BBitmap *mBitmap;
  TRect mRect;
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BConsoleFont32;
class BBitmap32;

class BViewPort32 : public BViewPort {
public:
  BViewPort32(const char *aName, BBitmap32 *aBitmap);
  ~BViewPort32();

public:
  inline void INLINE PlotPixel(TRGB &aColor, TUint32 aX, TUint32 aY) {
    mBitmap32->PlotPixel(aColor, mRect.x1 + aX, mRect.y1 + aY);
  }
  inline void INLINE PlotPixel(TUint8 aColor, TUint32 aX, TUint32 aY) {
    mBitmap32->PlotPixel(aColor, mRect.x1 + aX, mRect.y1 + aY);
  }
  inline void INLINE SafePlotPixel(TRGB &aColor, TInt32 aX, TInt32 aY) {
    mBitmap32->PlotPixel(aColor, mRect.x1 + aX, mRect.y1 + aY);
  }
  inline void INLINE SafePlotPixel(TUint32 aColor, TInt32 aX, TInt32 aY) {
    mBitmap32->SafePlotPixel(aColor, mRect.x1 + aX, mRect.y1 + aY);
  }
  inline TUint32 INLINE ReadPixel(TInt32 aX, TInt32 aY) {
    return mBitmap32->ReadPixel(mRect.x1 + aX, mRect.y1 + aY);
  }

public:
  void Clear(const TUint32 aColor);
  void Clear(TRGB &aColor) { return Clear(aColor.rgb888()); }
  //  void CopyPixels(BBitmap32 *aOther);

public:
  void FastLineHorizontal(TUint32 aColor, TInt aX, TInt aY, TUint aW);

  void FastLineVertical(TUint32 aColor, TInt aX, TInt aY, TUint aH);

  void DrawLine(TUint32 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2);
  void DrawLine(TRGB &aColor, TRect &aRect) {
    DrawLine(aColor.rgb888(), aRect.x1, aRect.y1, aRect.x2, aRect.y2);
  }

  void DrawRect(const TRGB &aColor, TRect &aRect) {
    DrawRect(aColor.rgb888(), aRect.x1, aRect.y1, aRect.x2, aRect.y2);
  }
  void DrawRect(TUint32 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2);

  void FillRect(TUint32 aColor, TInt aX1, TInt aY1, TInt aX2, TInt aY2);
  void FillRect(TRGB &aColor, TRect &aRect) {
    FillRect(aColor.rgb888(), aRect.x1, aRect.y1, aRect.x2, aRect.y2);
  }

public:
  void SetColors(TRGB &aForegroundColor, TRGB &aBackgroundColor) {
    mForegroundColor.Set(aForegroundColor);
    mBackgroundColor.Set(aBackgroundColor);
  }
  void GetColors(TRGB &aForegroundColor, TRGB &aBackgroundColor) {
    aForegroundColor.Set(mForegroundColor);
    aBackgroundColor.Set(mBackgroundColor);
  }

public:
  void SetFont(BConsoleFont32 *aFont) { mFont = aFont; }
  void DrawText(TInt16 aX, TInt16 aY, const char aString);
  void DrawText(TInt16 aX, TInt16 aY, const char *aString);
  void DrawTextTransparent(TInt16 aX, TInt16 aY, const char *aString);

public:
  void SetRect(TRect &aRect) {
    mRect.x1 = aRect.x1;
    mRect.y1 = aRect.y1;
    mRect.x2 = aRect.x2;
    mRect.y2 = aRect.y2;
  }

public:
  void Dump() {
    dlog("BViewPort32 at %x\n", this);
    mRect.Dump();
  }

protected:
  BBitmap32 *mBitmap32;
  TRGB mForegroundColor, mBackgroundColor;
  BConsoleFont32 *mFont;
};

#endif
