/*!
 * 6845 CRT Controller
 */

#ifndef SCREEN_H
#define SCREEN_H

#include <Exec/Types.h>
//#include "../x86/types.h"

const TInt VGA_WIDTH = 80;
const TInt VGA_HEIGHT = 25;
const TInt VGA_BYTESPERROW = VGA_WIDTH * 2;

const TUint8 BLACK = 0;
const TUint8 BLUE = 1;
const TUint8 GREEN = 2;
const TUint8 CYAN = 3;
const TUint8 RED = 4;
const TUint8 MAGENTA = 5;
const TUint8 BROWN = 6;
const TUint8 LTGREY = 7;

const TUint8 DKGREY = 0x08;
const TUint8 LTBLUE = 0x09;
const TUint8 LTGREEN = 0x0a;
const TUint8 LTCYAN = 0x0b;
const TUint8 LTRED = 0x0c;
const TUint8 PINK = 0x0d;
const TUint8 YELLOW = 0x0e;
const TUint8 WHITE = 0x0f;

class Screen {
public:
  Screen();

  void attr(TUint8 fg, TUint8 bg) {
    attribute = 0x0f;
//    attribute = ((bg << 4) & 0xf0) | (fg & 0x0f);
  }

  void moveto(int x, int y);
  void getxy(int &x, int &y);
  void down();

  // clear to end of line.  cursor is not moved.
  void cleareol(TUint8 ch = ' ');

  // scroll screen up one line starting at row y.  Fill in bottom row with bTInt
  void scrollup();

  void newline();
  void putc(char c);
  void cls(TUint8 ch = ' ');
  void puts(const char *s);
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
  char buf[256];
};

//extern Screen *gScreen;

#endif
