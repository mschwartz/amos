#include <stdarg.h>
#include "string.h"
#include "itoa.h"
#include "Screen.h"

extern Screen *screen;

void kprint(const char *fmt, ...) {
  va_list ap;
  char *s, c, t, tt;
  long d;
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
        switch (tt) {
          case '\0':
            break;
          case 's':
            s = va_arg(ap, char *);
            screen->puts(s);
            break;
          case 'd':
          case 'u':
            d = va_arg(ap, long);
            ltoa(d, buf, 10);
            screen->puts(buf);
            break;
          case 'x':
          case 'X':
            d = va_arg(ap, long);
            ltoa(d, buf, 16);
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
