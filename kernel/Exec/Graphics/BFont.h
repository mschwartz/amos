#ifndef EXEX_GRAPHICS_BFONT_H
#define EXEX_GRAPHICS_BFONT_H

#include <Graphics/Graphics.h>

// abstrct base class for BFront
class BFont : public BBase {
public:
  virtual void write(TInt x, TInt y, char c) = 0;
  virtual void write(TPoint &aPoint, char c) = 0;
  virtual void printf(TInt x, TInt y, const char *aFormat, ...) = 0;
  virtual void printf(TPoint &aPoint, const char *aFormat, ...) = 0;
};

//typedef struct {
//  TInt16 mNumLines;
//  TInt16 mLines[];
//} TVectorCharacterData;

class BBitmap;

class BVectorFont : public BFont {
public:
  // factory method
  static BVectorFont *CreateVectorFont(BBitmap *aBitmap, const TInt16 **aCharset = ENull);
public:
  BVectorFont(BBitmap *aBitmap, const TInt16 **aCharset);
  virtual ~BVectorFont() = 0;

public:
  void write(TInt x, TInt y, char c);
  void printf(TInt x, TInt y, const char *aFormat, ...);
  void write(TPoint &aPoint, char c);
  void printf(TPoint &aPoint, const char *aFormat, ...);
  virtual TInt print_string_rotated(TInt x, TInt y, TFloat angle, const char *aFormat, ...) = 0;
protected:
  BBitmap *mBitmap;
  const TInt16 **mCharset;
} PACKED;

class BVectorFont32 : public BVectorFont {
public:
  BVectorFont32(BBitmap *aBitmap, const TInt16 **aCharset = ENull);
  ~BVectorFont32();
  TInt print_string_rotated(TInt x, TInt y, TFloat angle, const char *aFormat, ...);
public:
  TInt16 mScale;
  TRGB mColor;
};

#endif
