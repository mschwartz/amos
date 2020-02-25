#include <stdarg.h>
#include "string.h"
#include "Screen.h"

extern Screen *screen;

void kprint(const char *fmt, ...) {
  va_list ap;
  char *s, c, t, tt;
  int d;
  char buf[20];

  va_start(ap, fmt);
  if (!fmt) {
    screen->puts("NULL FORMAT");
    return;
  }

  while (true) {
    t = *fmt++;
    switch (t) {
      case '\0':
        return;
      case '\n':
        screen->newline();
        break;
      case '%':
        tt = *fmt++;
//        screen->puts("t ");
//        buf[0] = '\'';
//        buf[1] = t;
//        buf[2] = '\'';
//        buf[3] = '\0';
//        screen->puts(buf);
//        screen->hexbyte(t);

//        screen->puts("    tt ");
//        buf[0] = tt;
//        buf[1] = '\0';
//        screen->puts(buf);
//        screen->hexbyte(tt);

//        screen->newline();
        switch (tt) {
          case '\0':
            break;
          case 's':
            s = va_arg(ap, char *);
            screen->puts(s);
            break;
          case 'd':
          case 'u':
            d = va_arg(ap, int);
            itoa(d, buf, 10);
            screen->puts(buf);
            break;
          case 'x':
          case 'X':
            d = va_arg(ap, int);
            itoa(d, buf, 16);
            screen->puts(buf);
            break;
          case 'c':
            c = (char)va_arg(ap, int);
            screen->putc(c);
            break;
          default:
            screen->putc('%');
            screen->putc(tt);
            break;
        }
        break;
      default:
        screen->putc(t);
        break;
    }
  }
}
