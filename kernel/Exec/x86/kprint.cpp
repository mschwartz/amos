#include <stdarg.h>
#include <posix/string.h>
#include <posix/sprintf.h>
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
  TUint64 flags = GetFlags();
  cli();

  char buf[512];
  va_list args;
  va_start(args, fmt);

  vsprintf(buf, fmt, args);
  dputs(buf);
  va_end(args);
  SetFlags(flags);
}
