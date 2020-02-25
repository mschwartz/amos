#ifndef SCREEN_H
#define SCREEN_H

#include "../x86/types.h"

const int VGA_WIDTH = 80;

const uint8_t BLACK = 0;
const uint8_t BLUE = 1;
const uint8_t GREEN = 2;
const uint8_t CYAN = 3;
const uint8_t RED = 4;
const uint8_t MAGENTA = 5;
const uint8_t BROWN = 6;
const uint8_t LTGREY = 7;

const uint8_t DKGREY = 0x08;
const uint8_t LTBLUE = 0x09;
const uint8_t LTGREEN = 0x0a;
const uint8_t LTCYAN = 0x0b;
const uint8_t LTRED = 0x0c;
const uint8_t PINK = 0x0d;
const uint8_t YELLOW = 0x0e;
const uint8_t WHITE = 0x0f;

class Screen {
public:
  Screen();

  void attr(uint8_t fg, uint8_t bg) {
//    attribute = 0x1f;
    attribute = ((bg << 4) & 0xf0) | (fg & 0x0f);
  }

  void moveto(int x, int y);
  void down();

  // clear to end of line.  cursor is not moved.
  void cleareol(uint8_t ch = ' ');

  // scroll screen up one line starting at row y.  Fill in bottom row with blanks
  void scrollup(int y);

  void newline();
  void putc(char c);
  void cls(uint8_t ch = ' ');
  void puts(const char *s);
  void hexnybble(const uint8_t n);
  void hexbyte(const uint8_t b);
  void hexword(const uint16_t w);
  void hexlong(const uint32_t l);
  void hexdump(const uint64_t addr, int count);

protected:
  uint8_t *screen;
  uint8_t attribute;
  short row, col;
  char buf[256];
};

extern Screen *screen;

#endif
