#include <Graphics/Graphics.h>
#include <Graphics/BFont.h>
#include <Graphics/font/BVectorFont.h>

#include <posix/string.h>
#include <stdarg.h>
#include <math.h>

BVectorFont *CreateVectorFont(BBitmap32 *aBitmap, const TInt16 **aCharset) {
  return new BVectorFont32(aBitmap, aCharset);
}

BVectorFont::BVectorFont(BBitmap32 *aBitmap, const TInt16 **aCharset) : mBitmap(aBitmap), mCharset(aCharset) {
  //
}

#if 0
BVectorFont32::BVectorFont32(BBitmap32 *aBitmap, TInt16 **aCharset) : BVectorFont(aBitmap, aCharset) {
  mScale = 0x100;
}

BVectorFont32::~BVectorFont32() {
}

void BVectorFont::Write(TInt x, TInt y, TInt16 c) {
}

void BVectorFont::Write(TPoint& aPoint, TInt16 c) {
  Write(aPoint.x, aPoint.y, c);
}
#endif
