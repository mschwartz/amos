#ifndef GRAPHICS_CONSOLE_FONT_H
#define GRAPHICS_CONSOLE_FONT_H

#include <Graphics/Graphics.h>
#include <Graphics/bitmap/BBitmap32.h>

typedef struct {
  TUint32 mMagic;
  TUint32 mVersion;
  TUint32 mHeaderSize;
  TUint32 mFlags;
  TUint32 mNumGlyphs;
  TUint32 mBytesPerGlyph;
  TUint32 mHeight;
  TUint32 mWidth;
} TConsoleFontHeader;

class BConsoleFont : public BFont {
  public:
    // factory
    BConsoleFont *CreateConsoleFont(BBitmap *aBitmap, const TInt16 **aCharset);

public:
  BConsoleFont(BBitmap *aBitmap, const TInt16 **aCharset);
   ~BConsoleFont() {};

public:
  TInt CharacterWidth() { return mFont->mWidth; }
  TInt CharacterHeight() { return mFont->mHeight; }

protected:
  BBitmap *mBitmap;
  const TInt16 **mCharset;
  TConsoleFontHeader *mFont;
} PACKED;

class BConsoleFont32 : public BConsoleFont {
public:
  BConsoleFont32(BBitmap *aBitmap, const TInt16 **aCharset = ENull);
  ~BConsoleFont32();
public:
  void Write(TInt aX, TInt y, TInt16 aChar);
  void Write(TPoint &aPoint, TInt16 aChar);
  void Write(TInt aX, TInt y, const char *aString);
  void Write(TPoint &aPoint, const char *aString);

protected:
  BBitmap32 *mBitmap32;
  TRGB mForegroundColor, mBackgroundColor;
};

#endif
