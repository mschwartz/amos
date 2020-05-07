#ifndef EXEX_GRAPHICS_BFONT_H
#define EXEX_GRAPHICS_BFONT_H

#include <Graphics/Graphics.h>
#include <Graphics/BBitmap.h>
#include <Exec/BList.h>
/**
  * BConsoleFont is a bitmapped glyph oriented font.  The fonts are found on a linux system in
  * /usr/share/kbd/consolefonts/ *.psf.    They may be gzipped on disk, but must be uncompressed
  * when included in the kernel.
  */
// abstrct base class for BFront
class BFont : public BNodePri {
public:
  BFont(const char *aName);
  ~BFont();

public:
  virtual TInt Write(TInt x, TInt y, TInt16 c) = 0;
  virtual TInt Write(TPoint &aPoint, TInt16 c) = 0;
  virtual TInt Write(TInt x, TInt y, const char *aString) = 0;
  virtual TInt Write(TPoint &aPoint, const char *aString) = 0;
  //  virtual TInt printf(TInt x, TInt y, const char *aFormat, ...) = 0;
  //  virtual TInt printf(TPoint &aPoint, const char *aFormat, ...) = 0;
};

//#include <Graphics/font/BVectorFont.h>
#include <Graphics/font/BConsoleFont.h>

#endif