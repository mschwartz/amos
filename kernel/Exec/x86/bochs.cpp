#include <stdarg.h>
#include <Exec/x86/bochs.h>
//#include <Exec/x86/kprint.h>
#include <posix/itoa.h>
#include <posix/sprintf.h>
#include <Exec/ExecBase.h>
#include <Devices/SerialDevice.h>
//#include <Screen.h>

extern "C" TUint8 bochs_present;

// TUint8 in_bochs; //  = *((TUint8 *)0x7c10);

//extern "C" TUint64 GetFlags();
//extern "C" void SetFlags(TUint64 aFlags);

extern "C" void eputs(const char *s);

extern "C" void sputc(char c);

void dassert(TBool aValue, const char *aFormat, ...) {
  if (aValue) {
    return;
  }
  DISABLE;

  va_list args;
  va_start(args, aFormat);

  char buf[512];
  dprint("%020d %-16s ASSERT FAILED!", gExecBase.SystemTicks(), gExecBase.CurrentTaskName());
  vsprintf(buf, aFormat, args);
  dputs(buf);
  va_end(args);

  bochs;
  ENABLE;
  
}

void dputc(char c) {
  //  sputc(c);
  //    outb((int)c, 0xe9);
  //  sputc(c);
  if (bochs_present) {
    outb(0xe9, (int)c);
  }
  else {
    sputc(c);
  }
}

void dputs(const char *s) {
  DISABLE;
  while (*s) {
    dputc(*s++);
  }
  ENABLE;
}

void dlog(const char *fmt, ...) {
  DISABLE;

  va_list args;
  va_start(args, fmt);

  char buf[512];
  dprint("%020d %-16s ", gExecBase.SystemTicks(), gExecBase.CurrentTaskName());
  vsprintf(buf, fmt, args);
  dputs(buf);
  va_end(args);

  ENABLE;
}

void dprintf(const char *fmt, ...) {
  char buf[512];
  va_list args;
  va_start(args, fmt);

  vsprintf(buf, fmt, args);
  dputs(buf);
  va_end(args);
}

void dprint(const char *fmt, ...) {
  char buf[512];
  va_list args;
  va_start(args, fmt);

  vsprintf(buf, fmt, args);
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

void dhexdump(const TAny *aSource, int aLines) {
  TUint8 *src = (TUint8 *)aSource;

  TUint64 address = (TUint64)src;
  for (TInt i = 0; i < aLines; i++) {
    dprint("%x: ", address);
    for (TInt c = 0; c < 8; c++) {
      dhex8(*src++);
      dputc(' ');
      address++;
    }
    dputc('\n');
  }
}

void dhexdump16(const TAny *aSource, int aLines) {
  TUint16 *src = (TUint16 *)aSource;
  TUint64 address = (TUint64)src;
  for (TInt i = 0; i < aLines; i++) {
    dprint("%x: ", address);
    for (TInt c = 0; c < 8; c++) {
      dhex16(*src++);
      dputc(' ');
      address += 2;
    }
    dputc('\n');
  }
}

void dhexdump32(const TAny *aSource, int aLines) {
  TUint32 *src = (TUint32 *)aSource;
  TUint64 address = (TUint64)src;
  for (TInt i = 0; i < aLines; i++) {
    dprint("%x: ", address);
    for (TInt c = 0; c < 8; c++) {
      dhex32(*src++);
      dputc(' ');
      address += 2;
    }
    dputc('\n');
  }
}

void dhexdump64(const TAny *aSource, int aLines) {
  TUint64 *src = (TUint64 *)aSource;
  TUint64 address = (TUint64)src;
  for (TInt i = 0; i < aLines; i++) {
    dprint("%x: ", address);
    for (TInt c = 0; c < 8; c++) {
      dhex64(*src++);
      dputc(' ');
      address += 2;
    }
    dputc('\n');
  }
}
