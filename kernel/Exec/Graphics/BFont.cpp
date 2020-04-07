#include <Exec/Graphics/BFont.h>
#include <Exec/Graphics/VectorFonts/Font.h>

#include <posix/string.h>
#include <stdarg.h>
#include <math.h>

BVectorFont *CreateVectorFont(BBitmap *aBitmap, const TInt16 **aCharset) {
  if (aCharset == ENull) {
    return new BVectorFont32(aBitmap, gVectorCharset);
  }
  return new BVectorFont32(aBitmap, aCharset);
}

BVectorFont::BVectorFont(BBitmap *aBitmap, const TInt16 **aCharset) : mBitmap(aBitmap), mCharset(aCharset) {
  //
}

BVectorFont32::BVectorFont32(BBitmap *aBitmap, const TInt16 **aCharset) : BVectorFont(aBitmap, aCharset) {
  mScale = 0x100;
  mColor = TRGB(255, 255, 255);
}

BVectorFont32::~BVectorFont32() {
}

void BVectorFont::write(TInt x, TInt y, char c) {

}

void BVectorFont::write(TPoint &aPoint, char c) {
  write(aPoint.x, aPoint.y, c);
}

void BVectorFont::printf(TInt x, TInt y, const char *aFormat, ...) {
}

void BVectorFont::printf(TPoint &aPoint, const char *aFormat, ...) {
}

TInt BVectorFont32::print_string_rotated(TInt x, TInt y, TFloat theta, const char *aFormat, ...) {
#if 0
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
#else
  return 0;
#endif
}
