#ifndef BFONT_VECTOR_FONT_H
#define BFONT_VECTOR_FONT_H

#include <Graphics/BFont.h>

class BBitmap32;

class BVectorFont : public BFont {
public:
  BVectorFont(BBitmap32 *aBitmap, const TInt16 **aCharset = ENull);
  virtual ~BVectorFont() = 0;

public:
  TInt Write(TInt aX, TInt aY, TInt16 aChar);
  TInt Write(TPoint &aPoint, TInt16 aChar);
  TInt Write(TInt aX, TInt aY, const char *aString);
  TInt Write(TPoint &aPoint, const char *aString);
//  virtual TInt PrintStringRotated(TInt x, TInt y, TFloat angle, const char *aString) = 0;
protected:
  BBitmap32 *mBitmap;
  const TInt16  * const *mCharset;
} PACKED;

/********************************************************************************
 ********************************************************************************
 ********************************************************************************/

class BVectorFont32 : public BVectorFont {
public:
//  BVectorFont32(BBitmap32 *aBitmap, const TInt16 **aCharset = ENull);
  BVectorFont32(BBitmap32 *aBitmap, const TInt16 **aCharset = ENull);
  ~BVectorFont32();
//  TInt PrintStringRotated(TInt x, TInt y, TFloat angle, const char *aString);
public:
  TInt16 mScale;
  TRGB mColor;
};

#endif


