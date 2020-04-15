#include <stdarg.h>
#include <posix/string.h>
#include <posix/itoa.h>
#include <Devices/Screen.h>
#include <Exec/ExecBase.h>

//extern Screen *screen;

void kputc(char c) {
  gExecBase.putc(c);
//  gScreen->putc(c);
}

void kputs(const char *s) {
  while (*s) {
    kputc(*s++);
  }
}

void kprint(const char *fmt, ...) {
  va_list ap;
  char *s, c, t, tt;
  long d;
  char buf[20];

  va_start(ap, fmt);
  if (!fmt) {
    gExecBase.puts("NULL FORMAT");
    return;
  }

  while (true) {
    t = *fmt++;
    switch (t) {
      case '\0':
        return;
      case '\n':
        gExecBase.newline();
        break;
      case '%':
        tt = *fmt++;
        switch (tt) {
          case '\0':
            break;
          case 's':
            s = va_arg(ap, char *);
            gExecBase.puts(s);
            break;
          case 'd':
          case 'u':
            d = va_arg(ap, long);
            ltoa(d, buf, 10);
            gExecBase.puts(buf);
            break;
          case 'x':
          case 'X':
            d = va_arg(ap, long);
            ltoa(d, buf, 16);
            gExecBase.puts(buf);
            break;
          case 'c':
            c = (char)va_arg(ap, int);
            gExecBase.putc(c);
            break;
          default:
            gExecBase.putc('%');
            gExecBase.putc(tt);
            break;
        }
        break;
      default:
        gExecBase.putc(t);
        break;
    }
  }
}
