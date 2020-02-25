#include <sys/io.h>
#include "Screen.h"
#include "../x86/kernel_memory.h"

Screen::Screen() {
  screen = (uint8_t *)0xb8000;
  row = col = 0;
  attr(BLACK, WHITE);
  cls();
  puts("Constructed\n");
}

void Screen::moveto(int x, int y) {
  uint16_t pos = y * VGA_WIDTH + x;
  outb(0x0f, 0x3d4);
  outb((uint8_t)(pos & 0xff), 0x3d5);
  outb(0x0e, 0x3d4);
  outb((uint8_t)((pos >> 8) & 0xff), 0x3d5);
}

void Screen::cleareol(uint8_t ch) {
  uint8_t *ptr = &screen[row * VGA_WIDTH + col];
  for (int c = col; c < VGA_WIDTH; col++) {
    *ptr++ = ch;
    *ptr++ = attribute;
  }
}

void Screen::scrollup(int y) {
  for (int r = y; r < 24; r++) {
    memcpy(&screen[r*VGA_WIDTH*2], &screen[(r+1)*VGA_WIDTH*2], 80*2);
    uint8_t *dst = &screen[r*VGA_WIDTH];
  }
  moveto(0, 24);
  cleareol();
}

void Screen::down() {
  row++;
  if (row > 24) {
    scrollup(0);
  }
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
    short offset = row * 160 + col * 2;


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

void Screen::hexdump(const uint64_t address, int count) {
  uint8_t *ptr = (uint8_t *)address;
  hexlong(address);
  putc(' ');
  for (int i=0; i<count; i++) {
    hexbyte(*ptr++);
    putc(' ');
  }
  newline();
}

Screen *screen;
