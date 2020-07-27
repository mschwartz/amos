#include <Graphics/Graphics.hpp>
#include <Graphics/font/BConsoleFont.hpp>
#include <posix/sprintf.h>
#include <limits.h>

// extern "C" TConsoleFontHeader _binary_cp866_8x16_psf_start;
// extern "C" TUint8 _binary_cp866_8x16_psf_end[];
extern "C" TConsoleFontHeader KFONTSTART;
extern "C" TUint8 KFONTEND[];

BConsoleFont32::BConsoleFont32(const TConsoleFont *aFont) {
  // dprint("BConsoleFont32 %x\n", this);
  if (aFont == ENull) {
    // mFont.mHeader = &_binary_cp866_8x16_psf_start;
    // mFont.mEnd = &_binary_cp866_8x16_psf_end[0];
    mFont.mHeader = &KFONTSTART;
    mFont.mEnd = &KFONTEND[0];
  }
  else {
    // copy
    mFont = *aFont;
  }
  mForegroundColor.Set(255, 255, 255);
  mBackgroundColor.Set(0, 0, 0);
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

TInt BConsoleFont32::Write(BBitmap32 *aBitmap, TInt aX, TInt aY, TInt16 aChar) {
  TConsoleFontHeader *h = mFont.mHeader;
  TInt pitch = (h->mWidth + 7) / 8;
  TUint32 fg = mForegroundColor.rgb888(),
          bg = mBackgroundColor.rgb888();

  TUint8 *glyph = (TUint8 *)h + h->mHeaderSize + (aChar > 0 && aChar < h->mNumGlyphs ? aChar : 0) * h->mBytesPerGlyph;
  for (TInt yy = 0; yy < h->mHeight; yy++) {
    TInt mask = 1 << (h->mWidth - 1);
    for (TInt xx = 0; xx < h->mWidth; xx++) {
      TUint32 color = *glyph & mask ? fg : bg;
      aBitmap->SafePlotPixel(color, aX + xx, aY + yy);
      mask >>= 1;
    }
    glyph = &glyph[pitch];
  }
  return aX + h->mWidth;
}

TInt BConsoleFont32::Write(BBitmap32 *aBitmap, TInt aX, TInt aY, const char *aString) {
  while (*aString) {
    aX = Write(aBitmap, aX, aY, *aString++);
  }
  return aX;
}

TInt BConsoleFont32::WriteTransparent(BBitmap32 *aBitmap, TInt aX, TInt aY, TInt16 aChar) {
  TConsoleFontHeader *h = mFont.mHeader;
  TInt pitch = (h->mWidth + 7) / 8;
  TUint32 fg = mForegroundColor.rgb888();

  TUint8 *glyph = (TUint8 *)h + h->mHeaderSize + (aChar > 0 && aChar < h->mNumGlyphs ? aChar : 0) * h->mBytesPerGlyph;
  for (TInt yy = 0; yy < h->mHeight; yy++) {
    TInt mask = 1 << (h->mWidth - 1);
    for (TInt xx = 0; xx < h->mWidth; xx++) {
      if (*glyph & mask) {
        aBitmap->SafePlotPixel(fg, aX + xx, aY + yy);
      }
      mask >>= 1;
    }
    glyph = &glyph[pitch];
  }
  return aX + h->mWidth;
}

TInt BConsoleFont32::WriteTransparent(BBitmap32 *aBitmap, TInt aX, TInt aY, const char *aString) {
  while (*aString) {
    aX = WriteTransparent(aBitmap, aX, aY, *aString++);
  }
  return aX;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

TInt BConsoleFont32::Write(BViewPort32 *aViewPort, TInt aX, TInt aY, TInt16 aChar) {
  TConsoleFontHeader *h = mFont.mHeader;
  TInt pitch = (h->mWidth + 7) / 8;
  TUint32 fg = mForegroundColor.rgb888(),
          bg = mBackgroundColor.rgb888();

  TUint8 *glyph = (TUint8 *)h + h->mHeaderSize + (aChar > 0 && aChar < h->mNumGlyphs ? aChar : 0) * h->mBytesPerGlyph;
  for (TInt yy = 0; yy < h->mHeight; yy++) {
    TInt mask = 1 << (h->mWidth - 1);
    for (TInt xx = 0; xx < h->mWidth; xx++) {
      TUint32 color = *glyph & mask ? fg : bg;
      aViewPort->SafePlotPixel(color, aX + xx, aY + yy);
      mask >>= 1;
    }
    glyph = &glyph[pitch];
  }
  return aX + h->mWidth;
}

TInt BConsoleFont32::Write(BViewPort32 *aViewPort, TInt aX, TInt aY, const char *aString) {
  while (*aString) {
    aX = Write(aViewPort, aX, aY, *aString++);
  }
  return aX;
}

TInt BConsoleFont32::WriteTransparent(BViewPort32 *aViewPort, TInt aX, TInt aY, TInt16 aChar) {
  TConsoleFontHeader *h = mFont.mHeader;
  TInt pitch = (h->mWidth + 7) / 8;
  TUint32 fg = mForegroundColor.rgb888();

  TUint8 *glyph = (TUint8 *)h + h->mHeaderSize + (aChar > 0 && aChar < h->mNumGlyphs ? aChar : 0) * h->mBytesPerGlyph;
  for (TInt yy = 0; yy < h->mHeight; yy++) {
    TInt mask = 1 << (h->mWidth - 1);
    for (TInt xx = 0; xx < h->mWidth; xx++) {
      if (*glyph & mask) {
        aViewPort->SafePlotPixel(fg, aX + xx, aY + yy);
      }
      mask >>= 1;
    }
    glyph = &glyph[pitch];
  }
  return aX + h->mWidth;
}

TInt BConsoleFont32::WriteTransparent(BViewPort32 *aViewPort, TInt aX, TInt aY, const char *aString) {
  while (*aString) {
    aX = Write(aViewPort, aX, aY, *aString++);
  }
  return aX;
}
