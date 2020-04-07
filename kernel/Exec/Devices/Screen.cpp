#include <Exec/Devices/Screen.h>
#include <x86/cpu.h>
#include <x86/bochs.h>
#include <x86/kprint.h>
#include <x86/kernel_memory.h>
#include "screen/VGAScreen.h"
#include "screen/BitmapScreen.h"

Screen *Screen::CreateScreen() {
  dprint("Create VGA Screen\n");
  return new VGAScreen();
}

void Screen::hexnybble(const TUint8 n) {
  const char *nybbles = "0123456789ABCDEF";
  putc(nybbles[n & 0x0f]);
}

void Screen::hexbyte(const TUint8 b) {
  hexnybble(b >> 4);
  hexnybble(b);
}

void Screen::hexword(const TUint16 w) {
  hexbyte(w >> 8);
  hexbyte(w);
}

void Screen::hexlong(const TUint32 l) {
  hexword((l >> 16) & 0xffff);
  hexword(l & 0xffff);
}

void Screen::hexquad(const TUint64 l) {
  hexlong((l >> 32) & 0xffffffff);
  hexlong(l & 0xffffffff);
}

void Screen::hexdump(const TAny *addr, TInt count) {
  TUint8 *ptr = (TUint8 *)addr;
  TUint64 address = (TUint64)addr;;
  hexlong(address);
  putc(' ');
  for (TInt i = 0; i < count; i++) {
    hexbyte(*ptr++);
    putc(' ');
  }
  newline();
}

Screen *gScreen;
