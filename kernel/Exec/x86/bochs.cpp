#include <stdarg.h>
#include <x86/bochs.h>
#include <x86/kprint.h>
#include <posix/itoa.h>
#include <posix/sprintf.h>
#include <Exec/ExecBase.h>
//#include <Screen.h>

TUint8 in_bochs; //  = *((TUint8 *)0x7c10);

extern "C" TUint64 GetFlags();
extern "C" void SetFlags(TUint64 aFlags);

extern "C" void eputs(const char *s);
void dputs(const char *s) {
  TUint64 flags = GetFlags();
  cli();
  eputs(s);
  SetFlags(flags);
}

void dlog(const char *fmt, ...) {
  TUint64 flags = GetFlags();
  char buf[512];
  va_list args;
  va_start(args, fmt);

  dprint("%020d ", gExecBase.SystemTicks());
  vsprintf(buf, fmt, args);
  dputs(buf);
  va_end(args);
  SetFlags(flags);
}

void dprintf(const char *fmt, ...) {
  TUint64 flags = GetFlags();
  char buf[512];
  va_list args;
  va_start(args, fmt);

  vsprintf(buf, fmt, args);
  dputs(buf);
  va_end(args);
  SetFlags(flags);
}

void dprint(const char *fmt, ...) {
  TUint64 flags = GetFlags();
  char buf[512];
  va_list args;
  va_start(args, fmt);

  vsprintf(buf, fmt, args);
  dputs(buf);
  va_end(args);
  SetFlags(flags);
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

void dhexdump(TUint8 *src, int lines) {
  TUint64 address = (TUint64)src;
  for (TInt i = 0; i < lines; i++) {
    dprint("%x: ", address);
    for (TInt c = 0; c < 8; c++) {
      dhex8(*src++);
      dputc(' ');
      address++;
    }
    dputc('\n');
  }
}

void dhexdump16(TUint16 *src, int lines) {
  TUint64 address = (TUint64)src;
  for (TInt i = 0; i < lines; i++) {
    dprint("%x: ", address);
    for (TInt c = 0; c < 8; c++) {
      dhex16(*src++);
      dputc(' ');
      address += 2;
    }
    dputc('\n');
  }
}
