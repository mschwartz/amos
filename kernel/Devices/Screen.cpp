#include <Devices/Screen.h>
#include <x86/cpu.h>
#include <x86/bochs.h>
#include <x86/kprint.h>
#include <x86/kernel_memory.h>

Screen::Screen() {
  screen = (TUint8 *)0xb8000;
  row = col = 0;
  attr(BLACK, WHITE);
  cls();
  dprint("Constructed Screen\n");
}

void Screen::moveto(int x, int y) {
  TUint16 pos = y * VGA_WIDTH + x;
  outb(0x0f, 0x3d4);
  outb((TUint8)(pos & 0xff), 0x3d5);
  outb(0x0e, 0x3d4);
  outb((TUint8)((pos >> 8) & 0xff), 0x3d5);
}

void Screen::getxy(int &x, int &y) {
  x = col;
  y = row;
}

void Screen::cleareol(TUint8 ch) {
  TUint16 *dst = (TUint16 *)&screen[row * VGA_BYTESPERROW],
           blank = (attribute << 8) | ch;
  for (int c = col; c < VGA_WIDTH; col++) {
    *dst++ = blank;
  }
}

void Screen::scrollup() {
  TUint16 *dst = (TUint16 *)&screen[0 * VGA_BYTESPERROW],
           *src = (TUint16 *)&screen[(0 + 1) * VGA_BYTESPERROW],
           blank = (attribute << 8) | ' ';

  for (TInt r = 0; r < 25; r++) {
    for (TInt c = 0; c < VGA_WIDTH; c++) {
      *dst++ = *src++;
    }
  }
  for (TInt c = 0; c < VGA_WIDTH; c++) {
    *dst++ = blank;
  }

  row = 24;
//  cleareol();
  moveto(0, 24);
}

void Screen::down() {
  row++;
  if (row > 24) {
    scrollup();
    row = 24;
  }
  moveto(col, row);
}
void Screen::newline() {
  col = 0;
  down();
}

void Screen::putc(char c) {
  if (c == 10) {
    col = 0;
    down();
  }
  else if (c == 13) {
    col = 0;
  }
  else {
    TInt64 offset = row * 160 + col * 2;

    screen[offset++] = c;
    screen[offset++] = attribute;
    col++;

    if (col > 79) {
      col = 0;
      down();
    }
  }
  moveto(col, row);
}

void Screen::cls(TUint8 ch) {
  for (TInt i = 0; i < 25 * 80; i++) {
    screen[2 * i] = ch;
    screen[2 * i + 1] = attribute;
  }
  row = col = 0;
  moveto(col, row);
}

void Screen::puts(const char *s) {
  while (*s) {
    putc(*s++);
  }
}

void Screen::hexnybble(const TUint8 n) {
  const char *nybbles = "0123456789ABCDEF";
  putc(nybbles[n & 0x0f]);
}

void Screen::hexbyte(const TUint8 b) {
  hexnybble(b >> 4);
  hexnybble(b);
}

void Screen::hexword(const TUint16 w) {
  hexbyte(w >> 8);
  hexbyte(w);
}

void Screen::hexlong(const TUint32 l) {
  hexword((l >> 16) & 0xffff);
  hexword(l & 0xffff);
}

void Screen::hexquad(const TUint64 l) {
  hexlong((l >> 32) & 0xffffffff);
  hexlong(l & 0xffffffff);
}

void Screen::hexdump(const TAny *addr, TInt count) {
  TUint8 *ptr = (TUint8 *)addr;
  TUint64 address = (TUint64)addr;;
  hexlong(address);
  putc(' ');
  for (TInt i = 0; i < count; i++) {
    hexbyte(*ptr++);
    putc(' ');
  }
  newline();
}

Screen *gScreen;
