#ifndef KERNEL_BOCHS_H
#define KERNEL_BOCHS_H

#include <cpu.h>

// debugging methods for running within bochs x86 emulator

// Is a useless instruction. Causes Bochs to break at the calling point
#define bochs asm volatile("xchg %bx, %bx;");

//outputs a character to the debug console
inline void dputc(char c) {
  outb((int)c, 0xe9);
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

void dhex8(const uint8_t b);
void dhex16(const uint16_t w);
void dhex32(const uint32_t w);
void dhex64(const uint64_t w);

void dhexdump(uint8_t *src, int lines);

#endif
