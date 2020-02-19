/*
 * main() entrypoint for our example OS-like application
 */

#include <stdint.h>
#include <sys/io.h>
#include <stdarg.h>

class Screen {
public:
  Screen() {
    screen = (uint8_t *)0xb8000;
    row = col = 0;
    cls();
    print("Constructed\n");
  }

  void moveto(int x, int y) {
    const int VGA_WIDTH = 80;

    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x0f, 0x3d4);
    outb((uint8_t)(pos & 0xff), 0x3d5);
    outb(0x0e, 0x3d4);
    outb((uint8_t)((pos >> 8) & 0xff), 0x3d5);
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

  void print(const char *s) {
    while (*s) {
      putc(*s++);
    }
  }

protected:
  uint8_t *screen;
  short row, col;
};

int main(void) {
  Screen screen;
//  screen.cls();
  for (int i = 0; i < 5; i++) {
    screen.print("Hello from kernel_main\r\n");
  }
  return 0;
}
