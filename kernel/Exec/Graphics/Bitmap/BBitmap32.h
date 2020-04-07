#ifndef EXEC_BITMAP_BBITMAP32_h
#define EXEC_BITMAP_BBITMAP32_h

#include <Graphics/BBitmap.h>
#include <Graphics/BFont.h>

class BBitmap32 : BBitmap {
public:
  BBitmap32(TInt aWidth, TInt aHeight, TInt aDepth, TAny *aMemory = ENull);
  ~BBitmap32();

public:
  void SetFont(BFont *aFont) { mFont = aFont; }

public:
  inline void INLINE PlotPixel(TInt aX, TInt aY, TRGB &aColor) {
    TUint32 *ptr = &mPixels32[aY * mWidth + aX];
    *ptr = aColor.rgb888();
  }
  void SafePlotPixel(TInt aX, TInt aY, TRGB &aColor) {
    if (aX < 0 || aX > mWidth || aY < 0 || aY > mHeight) {
      return;
    }
    PlotPixel(aX, aY, aColor);
  }
  void Clear(TRGB &aColor);
  void Line(TFloat aX1, TFloat aY1, TFloat aX2, TFloat aY2, TRGB& aColor);
  void FastHLine(TInt aX, TInt aY, TUint aW, TRGB &aColor);
  void FastVLine(TInt aX, TInt aY, TUint aH, TRGB &aColor);

  void DrawRect(TInt aX1, TInt aY1, TInt aX2, TInt aY2, TRGB& aColor);
  void DrawRect(TRect& aRect, TRGB& aColor);
  void FillRect(TInt aX1, TInt aY1, TInt aX2, TInt aY2, TRGB& aColor);
  void FillRect(TRect& aRect, TRGB& aColor);

  void DrawBitmap(BBitmap *srcBitmap, TRect& aSrcRect, TInt aDstX, TInt aDstY, TUint32 aFlags = 0);
  void DrawBitmapTransparent(BBitmap *srcBitmap, TRect& aSrcRect, TInt aDstX, TInt aDstY, TUint32 aFlags = 0);

  void DrawString(const char *aStr, const BFont *aFont, TInt aDx, TInt aDy, TRGB& aFgColor, TRGB& aBgColor);

  //  inline void INLINE PlotPixel(TPoint& aPoint, TRGB& aColor) {
  //    TUint32 *ptr = &mPixels32[aPoint.y * mWidth + aPoint.x];
  //    *ptr = aColor.rgb888();
  //  }
protected:
  TUint32 *mPixels32;
  BFont *mFont;
};

#endif
