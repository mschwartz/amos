/*
 * main() entrypoint for our example OS-like application
 */

#include <stdint.h>
#include <sys/io.h>
#include <stdarg.h>
#include "string.h"

class Screen {
public:
  Screen() {
    screen = (uint8_t *)0xb8000;
    row = col = 0;
    cls();
    puts("Constructed\n");
  }

  void moveto(int x, int y) {
    const int VGA_WIDTH = 80;

    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x0f, 0x3d4);
    outb((uint8_t)(pos & 0xff), 0x3d5);
    outb(0x0e, 0x3d4);
    outb((uint8_t)((pos >> 8) & 0xff), 0x3d5);
  }

  void newline() {
    col = 0;
    row++;
    moveto(col, row);
  }

  void putc(char c) {
    if (c == 10) {
      col = 0;
      row++;
    }
    else if (c == 13) {
      col = 0;
    }
    else {
      short offset = row * 160 + col * 2;

      screen[offset++] = c;
      screen[offset++] = 0x0f;
      col++;
      if (col > 79) {
        col = 0;
        row++;
      }
    }
    moveto(col, row);
  }

  void cls() {
    for (int i = 0; i < 25 * 80; i++) {
      screen[2 * i] = ' ';
      screen[2 * i + 1] = 0x0f;
    }
    row = col = 0;
    moveto(col, row);
  }

  void puts(const char *s) {
    while (*s) {
      putc(*s++);
    }
  }

  void hexnybble(const uint8_t n) {
    const char *nybbles = "0123456789ABCDEF";
    putc(nybbles[n & 0x0f]);
  }

  void hexbyte(const uint8_t b) {
    hexnybble(b >> 4);
    hexnybble(b);
  }

  void hexword(const uint16_t w) {
    hexbyte(w >> 8);
    hexbyte(w);
  }

  void hexlong(const uint32_t l) {
    hexword((l >> 16) & 0xffff);
    hexword(l & 0xffff);
  }

protected:
  uint8_t *screen;
  short row, col;
  char buf[256];
};

extern "C" {
int kernel_main(uint32_t ax);
void foo();
}

int kernel_main(uint32_t ax) {
  Screen screen;
  screen.cls();
  foo();
  //  screen.hexword(0xea5a);
  screen.hexlong(ax);
  screen.putc(' ');
  screen.hexlong(0xbeefdead);
  //  screen.hexbyte(ax);
  //  screen.hexbyte(ax>>8);
  //  screen.hexbyte(0xe5);
  //  screen.hexbyte(0xe5);
  screen.newline();
  for (int i = 0; i < 5; i++) {
    screen.puts("Hello from kernel_main\r\n");
  }
  return 0;
}

uint64_t memory_test() {
  uint8_t *ptr = (uint8_t *)0x10000;
  uint64_t size = 0;
  while (true) {
    uint8_t p = *ptr;
    *ptr = 0x5a;
    if (*ptr != 0x5a) {
      return size;
    }
    *ptr = 0xa5;
    if (*ptr != 0xa5) {
      return size;
    }
    *ptr = p;
    ptr += 4096;
    size += 4096;
  }
//  char buf[4096];
//  itoa(size, buf, 10);
//  screen.hexlong(size);
//  screen.putc(' ');
//  screen.puts(buf);
  return size;
}
