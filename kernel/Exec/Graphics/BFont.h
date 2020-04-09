#ifndef EXEX_GRAPHICS_BFONT_H
#define EXEX_GRAPHICS_BFONT_H

#include <Graphics/Graphics.h>

// BVectorFont is a scalable and rotatable vector font.  The downside is that no background pixels are cleared/set to bg color
class BVectorFont;
// BConsoleFont is a bitmapped glyph oriented font.  Foreground and background colors are honored.  The fonts are found
// on a linux system in /usr/share/kbd/consolefonts/*.psf.  They may be gzipped on disk, but must be uncompressed
// when included in the kernel.
class BConsoleFont;
class BBitmap;

// abstrct base class for BFront
class BFont : public BBase {
  public:
  // factory methods
  static BVectorFont *CreateVectorFont(BBitmap *aBitmap, const TAny *aCharset = ENull);
  static BConsoleFont *CreateConsoleFont(BBitmap *aBitmap, TAny *aCharset = ENull);
public:
  BFont();
  BFont(TRGB& aForeground, TRGB& aBackground);
public:
  virtual void Write(TInt aX, TInt aY, TInt16 aChar) = 0;
  virtual void Write(TPoint &aPoint, TInt16 aChar) = 0;
  virtual void Write(TInt aX, TInt aY, const char *) = 0;
  virtual void Write(TPoint &aPoint, const char *) = 0;
public:
  void SetForegroundColor(TRGB& aForeground) {
    mForegroundColor.Set(aForeground);
  }
  void SetBackgroundColor(TRGB& aBackground) {
    mBackgroundColor.Set(aBackground);
  }
  void SetColors(TRGB& aForeground, TRGB& aBackground) {
    SetForegroundColor(aForeground);
    SetBackgroundColor(aBackground);
  }
protected:
  TRGB mForegroundColor, mBackgroundColor;;
};

#include <Graphics/Font/BVectorFont.h>
#include <Graphics/Font/BConsoleFont.h>
#endif
