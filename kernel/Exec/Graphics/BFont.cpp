#include <Exec/Graphics/BFont.h>
#include <Exec/Graphics/Font/BVectorFont.h>

TRGB gColorWhite(255,255,255);
TRGB gColorBlack(0,0,0);
    
BFont::BFont(TRGB& aForeground, TRGB& aBackground) {
  SetForegroundColor(aForeground);
  SetBackgroundColor(aBackground);
}

BFont::BFont() {
  SetColors(gColorBlack, gColorWhite);
}

BVectorFont *CreateVectorFont(BBitmap *aBitmap, const TInt16 **aCharset) {
  if (aCharset == ENull) {
    return new BVectorFont32(aBitmap);
  }
  return new BVectorFont32(aBitmap, aCharset);
}

