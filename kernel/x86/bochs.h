#ifndef KERNEL_BOCHS_H
#define KERNEL_BOCHS_H

#include <x86/cpu.h>
#include <Exec/kprint.h>
// debugging methods for running within bochs x86 emulator

// Is a useless instruction. Causes Bochs to break at the calling point
#define bochs asm volatile("xchg %bx, %bx;");

extern TUint8 in_bochs;

//outputs a character to the debug console
inline void dputc(char c) {
    outb((int)c, 0xe9);
    return;
  if (in_bochs) {
    outb((int)c, 0xe9);
  }
  else {
    kputc(c);
  }
}

//stops simulation and breaks into the debug console
inline void dbreak() {
//  outw(0x8A00, 0x8A00);
//  outw(0x8A00, 0x08AE0);
  outw(0x8A00, 0x8a00);
  outw(0x08AE0, 0x8a00);
}

void dputs(const char *s);
void dprintf(const char *fmt, ...);
void dprint(const char *fmt, ...);

void dhex8(const TUint8 b);
void dhex16(const TUint16 w);
void dhex32(const TUint32 w);
void dhex64(const TUint64 w);

void dhexdump(TUint8 *src, int lines);
void dhexdump16(TUint16 *src, int lines);

#endif
