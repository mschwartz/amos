#include "VGAScreen.h"

VGAScreen::VGAScreen() { // } : Screen() {
  dprint(" ----- Constructed VGAScreen\n");
  screen = (TUint8 *)0xb8000;
  row = col = 0;
  attr(BLACK, WHITE);
  cls();
}

VGAScreen::~VGAScreen() {
  //
}

void VGAScreen::moveto(int x, int y) {
  TUint16 pos = y * VGA_WIDTH + x;
  outb(0x0f, 0x3d4);
  outb((TUint8)(pos & 0xff), 0x3d5);
  outb(0x0e, 0x3d4);
  outb((TUint8)((pos >> 8) & 0xff), 0x3d5);
}

void VGAScreen::getxy(int &x, int &y) {
  x = col;
  y = row;
}

void VGAScreen::down() {
  row++;
  if (row > 24) {
    scrollup();
    row = 24;
  }
  moveto(col, row);
}

void VGAScreen::cleareol(TUint8 ch) {
  TUint16 *dst = (TUint16 *)&screen[row * VGA_BYTESPERROW],
           blank = (attribute << 8) | ch;

  for (int c = col; c < VGA_WIDTH; col++) {
    *dst++ = blank;
  }
}

void VGAScreen::scrollup() {
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

void VGAScreen::putc(char c) {
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

void VGAScreen::newline() {
  col = 0;
  down();
}

void VGAScreen::cls(TUint8 ch) {
  for (TInt i = 0; i < 25 * 80; i++) {
    screen[2 * i] = ch;
    screen[2 * i + 1] = attribute;
  }
  row = col = 0;
  moveto(col, row);
}

void VGAScreen::puts(const char *s) {
  while (*s) {
    putc(*s++);
  }
}

