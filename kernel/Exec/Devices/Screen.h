/*!
 * 6845 CRT Controller
 */

#ifndef SCREEN_H
#define SCREEN_H

#include <Exec/BTypes.h>
#include <Exec/BBase.h>
#include <Exec/Graphics/BBitmap.h>
#include <Exec/Memory.h>
#include <x86/bochs.h>

typedef struct {
  TUint16 mWidth;
  TUint16 mHeight;
  TUint16 mDepth;
  TUint16 mPitch;
  TAny *mFrameBuffer;

  void Dump() {
    dprint("DisplayMode @ 0x%x\n", this);
    dprint("  mWidth:             %x %d\n", &mWidth, mWidth);
    dprint("  mHeight:            %x %d\n", &mHeight, mHeight);
    dprint("  mDepth:             %x %d\n", &mDepth, mDepth);
    dprint("  mPitch:             %x %d\n", &mPitch, mPitch);
    dprint("  mFrameBuffer:       %x 0x%x\n", &mFrameBuffer, mFrameBuffer);
    dprint(" %d 0x%x\n\n", sizeof(*this), sizeof(*this));
  }
} PACKED TDisplayMode;

class Screen : public BBase {
public:
  // factory
  static Screen *CreateScreen();
  static void DumpModes();
  static TDisplayMode& GetDisplayMode();

public:
  Screen();
  virtual ~Screen();

public:
  virtual void moveto(int x, int y) = 0;
  virtual void getxy(int &x, int &y) = 0;
  virtual void down() = 0;

  // clear to end of line.  cursor is not moved.
  virtual void cleareol(TUint8 ch = ' ') = 0;

  // scroll screen up one line starting at row y.  Fill in bottom row with bTInt
  virtual void scrollup() = 0;

  virtual void putc(char c) = 0;
  virtual void newline() = 0;
  virtual void cls(TUint8 ch = ' ') = 0;
  virtual void puts(const char *s) = 0;

  void hexnybble(const TUint8 n);
  void hexbyte(const TUint8 b);
  void hexword(const TUint16 w);
  void hexlong(const TUint32 l);
  void hexquad(const TUint64 l);
  void hexdump(const void *addr, TInt count);

protected:
  TUint8 *screen;
  TUint8 attribute;
  TInt16 row, col;
  TDisplayMode mDisplayMode;
  char buf[256];
};

extern Screen *gScreen;

#endif
