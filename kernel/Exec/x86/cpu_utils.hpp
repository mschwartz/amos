#ifndef AMOS_EXEC_X86_CPU_UTILS_H
#define AMOS_EXEC_X86_CPU_UTILS_H

#include <Types.hpp>

// TODO: move I/O to a separate file
extern "C" void cli();
extern "C" void sti();

static __inline void disable_interrupts() {
  __asm__ __volatile__("cli");
}
static inline void enable_interrupts() {
  __asm__ __volatile__("sti");
}

// https://wiki.osdev.org/Inline_Assembly/Examples
static inline void outb(TUint16 port, TUint8 val) {
  asm volatile("outb %0, %1"
               :
               : "a"(val), "Nd"(port));
  /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}
static inline void outw(TUint16 port, TUint16 val) {
  asm volatile("outw %0, %1"
               :
               : "a"(val), "Nd"(port));
  /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}
static inline void outl(TUint16 port, TUint32 val) {
  asm volatile("outl %0, %1"
               :
               : "a"(val), "Nd"(port));
  /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}
static inline TUint8 inb(TUint16 port) {
  TUint8 ret;
  asm volatile("inb %1, %0"
               : "=a"(ret)
               : "Nd"(port));
  return ret;
}
static inline TUint16 inw(TUint16 port) {
  TUint16 ret;
  asm volatile("inw %1, %0"
               : "=a"(ret)
               : "Nd"(port));
  return ret;
}
static inline TUint32 inl(TUint16 port) {
  TUint32 ret;
  asm volatile("inl %1, %0"
               : "=a"(ret)
               : "Nd"(port));
  return ret;
}

static __inline void insl(unsigned short int __port, void *__addr, unsigned long int __count) {
  __asm__ __volatile__("cld ; rep ; insl"
                       : "=D"(__addr), "=c"(__count)
                       : "d"(__port), "0"(__addr), "1"(__count));
}

static __inline void insw(unsigned short int __port, void *__addr, unsigned long int __count) {
  __asm__ __volatile__("cld ; rep ; insw"
                       : "=D"(__addr), "=c"(__count)
                       : "d"(__port), "0"(__addr), "1"(__count));
}

static __inline void outsw(unsigned short int __port, const void *__addr, unsigned long int __count) {
  __asm__ __volatile__("cld ; rep ; outsw"
                       : "=S"(__addr), "=c"(__count)
                       : "d"(__port), "0"(__addr), "1"(__count));
}

static __inline void outsl(unsigned short int __port, const void *__addr, unsigned long int __count) {
  __asm__ __volatile__("cld ; rep ; outsl"
                       : "=S"(__addr), "=c"(__count)
                       : "d"(__port), "0"(__addr), "1"(__count));
}

#if 0
static __inline unsigned char inb(unsigned short int __port) {
  unsigned char _v;
  __asm__ __volatile__("inb %w1,%0"
                       : "=a"(_v)
                       : "Nd"(__port));
  return _v;
}

static __inline unsigned char inb_p(unsigned short int __port) {
  unsigned char _v;
  __asm__ __volatile__("inb %w1,%0\noutb %%al,$0x80"
                       : "=a"(_v)
                       : "Nd"(__port));
  return _v;
}

static __inline unsigned short int inw(unsigned short int __port) {
  unsigned short _v;
  __asm__ __volatile__("inw %w1,%0"
                       : "=a"(_v)
                       : "Nd"(__port));
  return _v;
}

static __inline unsigned short int inw_p(unsigned short int __port) {
  unsigned short int _v;
  __asm__ __volatile__("inw %w1,%0\noutb %%al,$0x80"
                       : "=a"(_v)
                       : "Nd"(__port));
  return _v;
}

static __inline unsigned int inl(unsigned short int __port) {
  unsigned int _v;
  __asm__ __volatile__("inl %w1,%0"
                       : "=a"(_v)
                       : "Nd"(__port));
  return _v;
}

static __inline unsigned int inl_p(unsigned short int __port) {
  unsigned int _v;
  __asm__ __volatile__("inl %w1,%0\noutb %%al,$0x80"
                       : "=a"(_v)
                       : "Nd"(__port));
  return _v;
}

static __inline void outb(unsigned char __value, unsigned short int __port) {
  __asm__ __volatile__("outb %b0,%w1"
                       :
                       : "a"(__value), "Nd"(__port));
}

static __inline void outb_p(unsigned char __value, unsigned short int __port) {
  __asm__ __volatile__("outb %b0,%w1\noutb %%al,$0x80"
                       :
                       : "a"(__value),
                       "Nd"(__port));
}

static __inline void outw(unsigned short int __value, unsigned short int __port) {
  __asm__ __volatile__("outw %w0,%w1"
                       :
                       : "a"(__value), "Nd"(__port));
}

static __inline void outw_p(unsigned short int __value, unsigned short int __port) {
  __asm__ __volatile__("outw %w0,%w1\noutb %%al,$0x80"
                       :
                       : "a"(__value),
                       "Nd"(__port));
}

static __inline void outl(unsigned int __value, unsigned short int __port) {
  __asm__ __volatile__("outl %0,%w1"
                       :
                       : "a"(__value), "Nd"(__port));
}

static __inline void outl_p(unsigned int __value, unsigned short int __port) {
  __asm__ __volatile__("outl %0,%w1\noutb %%al,$0x80"
                       :
                       : "a"(__value),
                       "Nd"(__port));
}

static __inline void insb(unsigned short int __port, void *__addr, unsigned long int __count) {
  __asm__ __volatile__("cld ; rep ; insb"
                       : "=D"(__addr), "=c"(__count)
                       : "d"(__port), "0"(__addr), "1"(__count));
}

static __inline void insw(unsigned short int __port, void *__addr, unsigned long int __count) {
  __asm__ __volatile__("cld ; rep ; insw"
                       : "=D"(__addr), "=c"(__count)
                       : "d"(__port), "0"(__addr), "1"(__count));
}

static __inline void insl(unsigned short int __port, void *__addr, unsigned long int __count) {
  __asm__ __volatile__("cld ; rep ; insl"
                       : "=D"(__addr), "=c"(__count)
                       : "d"(__port), "0"(__addr), "1"(__count));
}

static __inline void outsb(unsigned short int __port, const void *__addr, unsigned long int __count) {
  __asm__ __volatile__("cld ; rep ; outsb"
                       : "=S"(__addr), "=c"(__count)
                       : "d"(__port), "0"(__addr), "1"(__count));
}

static __inline void outsw(unsigned short int __port, const void *__addr, unsigned long int __count) {
  __asm__ __volatile__("cld ; rep ; outsw"
                       : "=S"(__addr), "=c"(__count)
                       : "d"(__port), "0"(__addr), "1"(__count));
}

static __inline void outsl(unsigned short int __port, const void *__addr, unsigned long int __count) {
  __asm__ __volatile__("cld ; rep ; outsl"
                       : "=S"(__addr), "=c"(__count)
                       : "d"(__port), "0"(__addr), "1"(__count));
}
#endif

static __attribute__((noreturn)) inline void halt() {
  //  disable_interrupts();
  while (true) {
    __asm__ __volatile__("hlt");
  }
}

#endif
