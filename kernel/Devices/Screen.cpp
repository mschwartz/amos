#include <cpu.h>
#include <Screen.h>
#include <kprint.h>
#include <kernel_memory.h>

Screen::Screen() {
  screen = (uint8_t *)0xb8000;
  row = col = 0;
  attr(BLACK, WHITE);
  cls();
}

void Screen::moveto(int x, int y) {
  uint16_t pos = y * VGA_WIDTH + x;
  outb(0x0f, 0x3d4);
  outb((uint8_t)(pos & 0xff), 0x3d5);
  outb(0x0e, 0x3d4);
  outb((uint8_t)((pos >> 8) & 0xff), 0x3d5);
}

void Screen::getxy(int &x, int &y) {
  x = col;
  y = row;
}

void Screen::cleareol(uint8_t ch) {
  uint16_t *dst = (uint16_t *)&screen[row * VGA_BYTESPERROW],
           blank = (attribute << 8) | ch;
  for (int c = col; c < VGA_WIDTH; col++) {
    *dst++ = blank;
  }
}

void Screen::scrollup() {
  uint16_t *dst = (uint16_t *)&screen[0 * VGA_BYTESPERROW],
           *src = (uint16_t *)&screen[(0 + 1) * VGA_BYTESPERROW],
           blank = (attribute << 8) | ' ';

  for (int r = 0; r < 25; r++) {
    for (int c = 0; c < VGA_WIDTH; c++) {
      *dst++ = *src++;
    }
  }
  for (int c = 0; c < VGA_WIDTH; c++) {
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
    long offset = row * 160 + col * 2;

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

void Screen::cls(uint8_t ch) {
  for (int i = 0; i < 25 * 80; i++) {
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

void Screen::hexnybble(const uint8_t n) {
  const char *nybbles = "0123456789ABCDEF";
  putc(nybbles[n & 0x0f]);
}

void Screen::hexbyte(const uint8_t b) {
  hexnybble(b >> 4);
  hexnybble(b);
}

void Screen::hexword(const uint16_t w) {
  hexbyte(w >> 8);
  hexbyte(w);
}

void Screen::hexlong(const uint32_t l) {
  hexword((l >> 16) & 0xffff);
  hexword(l & 0xffff);
}

void Screen::hexquad(const uint64_t l) {
  hexlong((l >> 32) & 0xffffffff);
  hexlong(l & 0xffffffff);
}

void Screen::hexdump(const void *addr, int count) {
  uint8_t *ptr = (uint8_t *)addr;
  uint64_t address = (uint64_t)addr;;
  hexlong(address);
  putc(' ');
  for (int i = 0; i < count; i++) {
    hexbyte(*ptr++);
    putc(' ');
  }
  newline();
}

Screen *screen;
