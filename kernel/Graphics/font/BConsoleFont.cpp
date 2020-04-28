#include <Graphics/Graphics.h>
#include <Graphics/font/BConsoleFont.h>
#include <posix/sprintf.h>
#include <limits.h>

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

//BConsoleFont *BConsoleFont::CreateConsoleFont(BBitmap *aBitmap, const TInt16 **aCharset) {
//  switch (mBitmap->Depth()) {
//    case 32:
//      return new BConsoleFont32(aBitmap, aCharset);
//      break;
//    default:
//      break;
//  }
//  return ENull;
//}

extern "C" TConsoleFontHeader _binary_cp866_8x16_psf_start;
extern "C" TUint8 _binary_cp866_8x16_psf_end[];

BConsoleFont::BConsoleFont(BBitmap *aBitmap, const TConsoleFont *aFont) : BFont("Console Font") {
  mBitmap = aBitmap;
  TRect r;
  mBitmap->GetRect(r);
  r.Dump();
  if (aFont == ENull) {
    mFont.mHeader = &_binary_cp866_8x16_psf_start;
    mFont.mEnd = &_binary_cp866_8x16_psf_end[0];
  }
  else {
    // copy
    mFont = *aFont;
  }
}

BConsoleFont32::BConsoleFont32(BBitmap32 *aBitmap, const TConsoleFont *aFont) {
  dprint("BConsoleFont32 %x\n", this);
  mBitmap32 = aBitmap;
  if (aFont == ENull) {
    mFont.mHeader = &_binary_cp866_8x16_psf_start;
    mFont.mEnd = &_binary_cp866_8x16_psf_end[0];
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

TInt BConsoleFont32::Write(TInt aX, TInt aY, TInt16 aChar) {
  return Write(mBitmap32, aX, aY, aChar);
}

TInt BConsoleFont32::Write(TInt aX, TInt aY, const char *aString) {
  while (*aString) {
    aX = Write(aX, aY, *aString++);
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

