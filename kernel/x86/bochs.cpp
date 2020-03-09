#include "bochs.h"
#include "kprint.h"
#include <stdarg.h>
#include <itoa.h>
#include <sprintf.h>
#include <Screen.h>

void dputs(const char *s) {
  while (*s) {
    dputc(*s++);
  }
}

void dprintf(const char *fmt, ...) {
  va_list ap;
  char *s, c, t, tt;
  int d;
  char buf[20];

  va_start(ap, fmt);
  if (!fmt) {
    dputs("NULL FORMAT");
    return;
  }

  while (true) {
    t = *fmt++;
    switch (t) {
      case '\0':
        return;
        //      case '\n':
        //        screen->newline();
        //        break;
      case '%':
        tt = *fmt++;
        switch (tt) {
          case '\0':
            break;
          case 's':
            s = va_arg(ap, char *);
            dputs(s);
            break;
          case 'd':
          case 'u':
            d = va_arg(ap, long);
            itoa(d, buf, 10);
            dputs(buf);
            //            screen->puts(buf);
            break;
          case 'x':
          case 'X':
            d = va_arg(ap, long);
            ltoa(d, buf, 16);
            dputs(buf);
            break;
          case 'c':
            c = (char)va_arg(ap, int);
            dputc(c);
            break;
          default:
            dputc('%');
            dputc(tt);
            break;
        }
        break;
      default:
        dputc(t);
        //        screen->putc(t);
        break;
    }
  }
}
void dprint(const char *fmt, ...) {
  char buf[512];
  va_list args;
  va_start(args, fmt);

  //  dprintf("args: %x\n", args);
  vsprintf(buf, fmt, args);
  //  vsprintf(buf, fmt, parameters);
//  kprint("%s", buf);
  dputs(buf);
  va_end(args);
}

void dhex4(const uint8_t n) {
  const char *nybbles = "0123456789ABCDEF";
  dputc(nybbles[n & 0x0f]);
}

void dhex8(const uint8_t b) {
  dhex4(b >> 4);
  dhex4(b);
}

void dhex16(const uint16_t w) {
  dhex8(w >> 8);
  dhex8(w);
}

void dhex32(const uint32_t l) {
  dhex16((l >> 16) & 0xffff);
  dhex16(l & 0xffff);
}

void dhex64(const uint64_t l) {
  dhex32((l >> 32) & 0xffffffff);
  dhex32(l & 0xffffffff);
}
