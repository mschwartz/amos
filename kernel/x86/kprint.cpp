#include <stdarg.h>
#include <posix/string.h>
#include <posix/itoa.h>
#include <Devices/Screen.h>

//extern Screen *screen;

void kprint(const char *fmt, ...) {
  va_list ap;
  char *s, c, t, tt;
  long d;
  char buf[20];

  va_start(ap, fmt);
  if (!fmt) {
    gScreen->puts("NULL FORMAT");
    return;
  }

  while (true) {
    t = *fmt++;
    switch (t) {
      case '\0':
        return;
      case '\n':
        gScreen->newline();
        break;
      case '%':
        tt = *fmt++;
        switch (tt) {
          case '\0':
            break;
          case 's':
            s = va_arg(ap, char *);
            gScreen->puts(s);
            break;
          case 'd':
          case 'u':
            d = va_arg(ap, long);
            ltoa(d, buf, 10);
            gScreen->puts(buf);
            break;
          case 'x':
          case 'X':
            d = va_arg(ap, long);
            ltoa(d, buf, 16);
            gScreen->puts(buf);
            break;
          case 'c':
            c = (char)va_arg(ap, int);
            gScreen->putc(c);
            break;
          default:
            gScreen->putc('%');
            gScreen->putc(tt);
            break;
        }
        break;
      default:
        gScreen->putc(t);
        break;
    }
  }
}
