/*!
 * 6845 CRT Controller
 */

#ifndef SCREEN_H
#define SCREEN_H

#include <Exec/BTypes.h>
#include <Exec/BBase.h>
#include <Exec/Graphics/BBitmap.h>
#include <Exec/Memory.h>

typedef struct {
  TUint16 mMode;
  TUint16 mWidth;
  TUint16 mHeight;
  TUint16 mPitch;
  TUint16 mBitsPerPixel;
  TUint16 mPad;
  void Dump() {
    dprint("Mode %x %d x %d %d bpp\n", mMode, mWidth, mHeight, mBitsPerPixel);
  }
} PACKED TModeInfo;

typedef struct {
  TInt16 mCount;          // number of modes found
  TModeInfo mDisplayMode; // chosen display mode
  TModeInfo mModes[];
  void Dump() {
    dprint("Found %d %x modes\n", mCount, mCount);
    for (TInt16 i = 0; i < mCount; i++) {
      mModes[i].Dump();
    }
  }
} PACKED TModes;
static TModes *gDisplayModes = (TModes *)0x5000;

class Screen : public BBase {
public:
  // factory
  static Screen *CreateScreen();

public:
//  Screen(); //   {}
//  virtual ~Screen() {}

public:
  virtual void moveto(int x, int y) = 0;
  virtual void getxy(int &x, int &y) = 0;
  virtual void down();

  // clear to end of line.  cursor is not moved.
  virtual void cleareol(TUint8 ch = ' ') = 0;

  // scroll screen up one line starting at row y.  Fill in bottom row with bTInt
  virtual void scrollup() = 0;

  virtual void newline() = 0;
  virtual void putc(char c) = 0;
  virtual void cls(TUint8 ch = ' ') = 0;
  virtual void puts(const char *s) = 0;

   void hexnybble(const TUint8 n);
   void hexbyte(const TUint8 b) ;
   void hexword(const TUint16 w);
   void hexlong(const TUint32 l);
   void hexquad(const TUint64 l);
   void hexdump(const void *addr, TInt count);

protected:
  TUint8 *screen;
  TUint8 attribute;
  TInt16 row, col;
  char buf[256];
};

extern Screen *gScreen;

#endif
