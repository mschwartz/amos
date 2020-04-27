#ifndef EXEX_GRAPHICS_BFONT_H
#define EXEX_GRAPHICS_BFONT_H

#include <Graphics/Graphics.h>

// abstrct base class for BFront
class BFont : public BBase {
public:
  virtual void Write(TInt x, TInt y, TInt16 c) = 0;
  virtual void Write(TPoint &aPoint, TInt16 c) = 0;
  virtual void Write(TInt x, TInt y, const char *aString) = 0;
  virtual void Write(TPoint &aPoint, const char *aString) = 0;
//  virtual void printf(TInt x, TInt y, const char *aFormat, ...) = 0;
//  virtual void printf(TPoint &aPoint, const char *aFormat, ...) = 0;
};

#endif
