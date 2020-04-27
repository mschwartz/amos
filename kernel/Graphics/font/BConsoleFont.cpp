#include <Graphics/Graphics.h>
#include <Graphics/font/BConsoleFont.h>
//#include <Exec/Graphics/BBitmap.h>

/*
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
*/

BConsoleFont *BConsoleFont::CreateConsoleFont(BBitmap *aBitmap, const TInt16 **aCharset) {
  switch (mBitmap->Depth()) {
    case 32:
      return new BConsoleFont32(aBitmap, aCharset);
      break;
    default:
      break;
  }
  return ENull;
}

BConsoleFont::BConsoleFont(BBitmap *aBitmap, const TInt16 **aCharset) : BFont() {
  mBitmap = aBitmap;
  mCharset = aCharset;
}

BConsoleFont32::BConsoleFont32(BBitmap *aBitmap, const TInt16 **aCharset) : BConsoleFont(aBitmap, aCharset) {
  mBitmap32 = (BBitmap32 *)aBitmap;
}

void BConsoleFont32::Write(TInt aX, TInt y, TInt16 aChar) {
  TUint8 *glyphs = (TUint8 *)mFont,
         *glyph = &glyphs[aChar * mFont->mBytesPerGlyph];

  // TODO: this can be optimized!
  for (TUint32 y = 0; y < mFont->mHeight; y++) {
    TUint8 b = *glyph++;
    for (TUint32 x = 0; x < 8; x++) {
      if (b & (1 << (8 - x))) {
        mBitmap32->SafePlotPixel(mForegroundColor, x, y);
      }
    }
  }
}

void BConsoleFont32::Write(TPoint &aPoint, TInt16 aChar) {
  Write(aPoint.x, aPoint.y, aChar);
}

void BConsoleFont32::Write(TInt aX, TInt aY, const char *aString) {
  while (*aString) {
    Write(aX, aY, *aString++);
    aX += mFont->mWidth;
  }
}

void BConsoleFont32::Write(TPoint &aPoint, const char *aString) {
  Write(aPoint.x, aPoint.y, aString);
}
