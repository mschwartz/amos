#ifndef GRAPHICS_CONSOLE_FONT_H
#define GRAPHICS_CONSOLE_FONT_H

#include <Graphics/Graphics.h>
#include <Graphics/font/BConsoleFont.h>
#include <Graphics/BBitmap.h>
#include <Graphics/BViewPort.h>
#include <Graphics/BFont.h>

class BBitmap32;
class BViewPort32;

typedef struct {
  TUint32 mMagic;
  TUint32 mVersion;
  TUint32 mHeaderSize;
  TUint32 mFlags;
  TUint32 mNumGlyphs;
  TUint32 mBytesPerGlyph;
  TUint32 mHeight;
  TUint32 mWidth;
} PACKED TConsoleFontHeader;

typedef struct {
  TConsoleFontHeader *mHeader;
  TUint8 *mEnd;
} TConsoleFont;

class BConsoleFont : public BFont {
public:
  // factory
  //    BConsoleFont *CreateConsoleFont(BBitmap32 *aBitmap, const TInt16 **aCharset);

public:
  BConsoleFont(BBitmap *aBitmap, const TConsoleFont *aFont);
  ~BConsoleFont(){};

public:
  TInt CharacterWidth() { return mFont.mHeader->mWidth; }
  TInt CharacterHeight() { return mFont.mHeader->mHeight; }

protected:
  BBitmap *mBitmap;
  TConsoleFont mFont;
} PACKED;

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BConsoleFont32 : public BBase {
public:
  BConsoleFont32(BBitmap32 *aBitmap, const TConsoleFont *aFont = ENull);
  ~BConsoleFont32();

public:
  TInt CharacterWidth() { return mFont.mHeader->mWidth; }
  TInt CharacterHeight() { return mFont.mHeader->mHeight; }

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
  TInt Write(TInt aX, TInt y, TInt16 aChar);
  TInt Write(TPoint &aPoint, TInt16 aChar);
  TInt Write(TInt aX, TInt y, const char *aString);
  TInt Write(TPoint &aPoint, const char *aString);

public:
  TInt Write(BBitmap32 *aBitmap, TInt aX, TInt aY, TInt16 aChar);
  TInt Write(BBitmap32 *aBitmap, TPoint &aPoint, TInt16 aChar) { return Write(aBitmap, aPoint.x, aPoint.y, aChar); }
  TInt Write(BBitmap32 *aBitmap, TInt aX, TInt aY, const char *aString);
  TInt Write(BBitmap32 *aBitmap, TPoint &aPoint, const char *aString) { return Write(aBitmap, aPoint.x, aPoint.y, aString); }

public:
  TInt Write(BViewPort32 *aViewPort, TInt aX, TInt aY, TInt16 aChar);
  TInt Write(BViewPort32 *aViewPort, TPoint &aPoint, TInt16 aChar) { return Write(aPoint.x, aPoint.y, aChar); }
  TInt Write(BViewPort32 *aViewPort, TInt aX, TInt aY, const char *aString);
  TInt Write(BViewPort32 *aViewPort, TPoint &aPoint, const char *aString) { return Write(aPoint.x, aPoint.y, aString); }

protected:
  BBitmap32 *mBitmap32;
  TRGB mForegroundColor, mBackgroundColor;
  TConsoleFont mFont;
};

#endif
