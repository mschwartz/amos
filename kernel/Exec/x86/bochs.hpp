#ifndef KERNEL_BOCHS_H
#define KERNEL_BOCHS_H

#if 0
#include <Exec/ExecBase.h>
#include <x86/cpu.h>
#include <x86/kprint.h>
// debugging methods for running within bochs x86 emulator

// Is a useless instruction. Causes Bochs to break at the calling point
#define bochs asm volatile("xchg %bx, %bx;");

extern TUint8 in_bochs;

//stops simulation and breaks into the debug console
inline void dbreak() {
  //  outw(0x8A00, 0x8A00);
  //  outw(0x8A00, 0x08AE0);
  outw(0x8A00, 0x8a00);
  outw(0x08AE0, 0x8a00);
}

extern void dputc(char c);
extern void dlog(const char *fmt, ...);
extern void dputs(const char *s);
extern void dprintf(const char *fmt, ...);
extern void dprint(const char *fmt, ...);

extern void dhex8(const TUint8 b);
extern void dhex16(const TUint16 w);
extern void dhex32(const TUint32 w);
extern void dhex64(const TUint64 w);

extern void dhexdump(TUint8 *src, int lines);
extern void dhexdump16(TUint16 *src, int lines);
extern void dhexdump32(TUint32 *src, int lines);

#endif

#endif
