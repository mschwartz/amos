#include <stdarg.h>
#include <x86/bochs.h>
#include <x86/kprint.h>
#include <posix/itoa.h>
#include <posix/sprintf.h>
//#include <Screen.h>

TUint8 in_bochs; //  = *((TUint8 *)0x7c10);

void dputs(const char *s) {
//  dprint("bochs %x\n", in_bochs);
  while (*s) {
    dputc(*s++);
  }
}

void dprintf(const char *fmt, ...) {
  va_list ap;
  char *s, c, t, tt;
  TInt d;
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
      case '%':
        tt = *fmt++;
        switch (tt) {
          case '\0':
            break;
          case 's':
            s = va_arg(ap, char *);
            if (in_bochs) {
            dputs(s);
            }
            else {
              kputs(s);
            }

            break;
          case 'd':
          case 'u':
            d = va_arg(ap, TUint64);
            itoa(d, buf, 10);
            dputs(buf);
            //            screen->puts(buf);
            break;
          case 'x':
          case 'X':
            d = va_arg(ap, TUint64);
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

void dhex4(const TUint8 n) {
  const char *nybbles = "0123456789ABCDEF";
  dputc(nybbles[n & 0x0f]);
}

void dhex8(const TUint8 b) {
  dhex4(b >> 4);
  dhex4(b);
}

void dhex16(const TUint16 w) {
  dhex8(w >> 8);
  dhex8(w);
}

void dhex32(const TUint32 l) {
  dhex16((l >> 16) & 0xffff);
  dhex16(l & 0xffff);
}

void dhex64(const TUint64 l) {
  dhex32((l >> 32) & 0xffffffff);
  dhex32(l & 0xffffffff);
}

void dhexdump(TUint8 *src, int lines){
  TUint64 address = (TUint64)src;
  for (TInt i=0; i<lines; i++) {
    dprint("%x: ", address);
    for (TInt c=0; c<8; c++) {
      dhex8(*src++);
      dputc(' ');
      address++;
    }
    dputc('\n');
  }
}

void dhexdump16(TUint16 *src, int lines){
  TUint64 address = (TUint64)src;
  for (TInt i=0; i<lines; i++) {
    dprint("%x: ", address);
    for (TInt c=0; c<8; c++) {
      dhex16(*src++);
      dputc(' ');
      address += 2;
    }
    dputc('\n');
  }
}

