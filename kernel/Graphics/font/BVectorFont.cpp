#include <Graphics/Graphics.h>
#include <Graphics/font/BVectorFont.h>
#include <Graphics/bitmap/BBitmap32.h>

#include "vectorfonts/Font.h"

#include <posix/string.h>
#include <stdarg.h>
#include <math.h>

BVectorFont::BVectorFont(BBitmap32 *aBitmap, const TInt16 **aCharset) : BFont("Vector Font"), mBitmap(aBitmap) {
  mCharset = aCharset;
  if (mCharset == ENull) {
    mCharset = gVectorCharset;
  }
}

BVectorFont32::BVectorFont32(BBitmap32 *aBitmap, const TInt16 **aCharset) : BVectorFont(aBitmap, aCharset) {
  mScale = 0x100;
  mColor = TRGB(255, 255, 255);
}

BVectorFont32::~BVectorFont32() {
}

TInt BVectorFont::Write(TInt aX, TInt aY, TInt16 aChar) {
  return 8;
}

TInt BVectorFont::Write(TPoint &aPoint, TInt16 aChar) {
  Write(aPoint.x, aPoint.y, aChar);
  return 8;
}

TInt BVectorFont::Write(TInt aX, TInt aY, const char *aString) {
  return 8;
}

TInt BVectorFont::Write(TPoint &aPoint, const char *aString) {
  Write(aPoint.x, aPoint.y, aString);
  return 8;
}

#if 0
TInt BVectorFont32::PrintStringRotated(TInt aX, TInt aY, TFloat aAngle, const char *aString) {

  theta = float(theta) * 3.1415926 / 180;
  TFloat cost = cos(theta),
        sint = sin(theta);

  TFloat fscale = TFloat(scale >> 8) + TFloat(scale & 0xff) / 256.0;

  const TInt8 size = 9;

  TInt8 xo = x;
  while (char c = *p++) {
    PGM_P glyph = (PGM_P)pgm_read_word(&charset[toupper(c) - 32]);
    if (glyph) {
      TInt8 lines = pgm_read_byte(glyph++);

      for (TInt8 i = 0; i < lines; i++) {
        TFloat x0 = (TInt8)pgm_read_byte(glyph++) * fscale + x,
              y0 = (TInt8)pgm_read_byte(glyph++) * fscale + y,
              x1 = (TInt8)pgm_read_byte(glyph++) * fscale + x,
              y1 = (TInt8)pgm_read_byte(glyph++) * fscale + y;

        Graphics::drawLine(
            x0,
            ((y0 - y) * sint + cost + y),
            x1,
            ((y1 - y) * sint + cost + y));
      }
      x += size * fscale;
    }
    else {
      x += 6 * fscale;
    }
  }
  return x - xo;
}
#endif
