#ifndef KERNEL_X86_CPU_H
#define KERNEL_X86_CPU_H

#include <Exec/Types.h>

extern void dprint(const char *fmt, ...);
extern "C" TUint64 GetCS();
extern "C" TUint64 GetDS();
extern "C" TUint64 GetES();
extern "C" TUint64 GetFS();
extern "C" TUint64 GetGS();
extern "C" TUint64 GetSS();

extern "C" TUint64 GetFlags();
extern "C" void SetFlags(TUint64 aFlags);

#define DISABLE TUint64 ___flags = GetFlags(); cli();
#define ENABLE SetFlags(___flags);

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

extern "C" TUint64 GetCR4();
const TUint64 CR4_CME = 1<<0;
const TUint64 CR4_PVI = 1<<1;
const TUint64 CR4_TSD = 1<<2;
const TUint64 CR4_DE = 1<<3;
const TUint64 CR4_PSE = 1<<4;
const TUint64 CR4_PAE = 1<<5;
const TUint64 CR4_MCE = 1<<6;
const TUint64 CR4_PGE = 1<<7;
const TUint64 CR4_PCE = 1<<8;
const TUint64 CR4_OSFXSR = 1<<9;
const TUint64 CR4_OSXMMEXCPT = 1<<10;
const TUint64 CR4_UMIP = 1<<12;
const TUint64 CR4_VMXE = 1<<13;
const TUint64 CR4_SMXE = 1<<14;
const TUint64 CR4_PCIDE = 1<<17;
const TUint64 CR4_OSXSAVE = 1<<18;
const TUint64 CR4_SMEP = 1<<20;
const TUint64 CR4_SMAP = 1<<21;
/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

const TUint64 CPUID_FEAT_ECX_SSE3         = 1 << 0;
const TUint64 CPUID_FEAT_ECX_PCLMUL       = 1 << 1;
const TUint64 CPUID_FEAT_ECX_DTES64       = 1 << 2;
const TUint64 CPUID_FEAT_ECX_MONITOR      = 1 << 3;
const TUint64 CPUID_FEAT_ECX_DS_CPL       = 1 << 4;
const TUint64 CPUID_FEAT_ECX_VMX          = 1 << 5;
const TUint64 CPUID_FEAT_ECX_SMX          = 1 << 6;
const TUint64 CPUID_FEAT_ECX_EST          = 1 << 7;
const TUint64 CPUID_FEAT_ECX_TM2          = 1 << 8;
const TUint64 CPUID_FEAT_ECX_SSSE3        = 1 << 9;
const TUint64 CPUID_FEAT_ECX_CID          = 1 << 10;
const TUint64 CPUID_FEAT_ECX_FMA          = 1 << 12;
const TUint64 CPUID_FEAT_ECX_CX16         = 1 << 13;
const TUint64 CPUID_FEAT_ECX_ETPRD        = 1 << 14;
const TUint64 CPUID_FEAT_ECX_PDCM         = 1 << 15;
const TUint64 CPUID_FEAT_ECX_PCIDE        = 1 << 17;
const TUint64 CPUID_FEAT_ECX_DCA          = 1 << 18;
const TUint64 CPUID_FEAT_ECX_SSE4_1       = 1 << 19;
const TUint64 CPUID_FEAT_ECX_SSE4_2       = 1 << 20;
const TUint64 CPUID_FEAT_ECX_X2APIC       = 1 << 21;
const TUint64 CPUID_FEAT_ECX_MOVBE        = 1 << 22;
const TUint64 CPUID_FEAT_ECX_POPCNT       = 1 << 23;
const TUint64 CPUID_FEAT_ECX_AES          = 1 << 25;
const TUint64 CPUID_FEAT_ECX_XSAVE        = 1 << 26;
const TUint64 CPUID_FEAT_ECX_OSXSAVE      = 1 << 27;
const TUint64 CPUID_FEAT_ECX_AVX          = 1 << 28;

const TUint64 CPUID_FEAT_EDX_FPU          = 1 << 0;
const TUint64 CPUID_FEAT_EDX_VME          = 1 << 1;
const TUint64 CPUID_FEAT_EDX_DE           = 1 << 2;
const TUint64 CPUID_FEAT_EDX_PSE          = 1 << 3;
const TUint64 CPUID_FEAT_EDX_TSC          = 1 << 4;
const TUint64 CPUID_FEAT_EDX_MSR          = 1 << 5;
const TUint64 CPUID_FEAT_EDX_PAE          = 1 << 6;
const TUint64 CPUID_FEAT_EDX_MCE          = 1 << 7;
const TUint64 CPUID_FEAT_EDX_CX8          = 1 << 8;
const TUint64 CPUID_FEAT_EDX_APIC         = 1 << 9;
const TUint64 CPUID_FEAT_EDX_SEP          = 1 << 11;
const TUint64 CPUID_FEAT_EDX_MTRR         = 1 << 12;
const TUint64 CPUID_FEAT_EDX_PGE          = 1 << 13;
const TUint64 CPUID_FEAT_EDX_MCA          = 1 << 14;
const TUint64 CPUID_FEAT_EDX_CMOV         = 1 << 15;
const TUint64 CPUID_FEAT_EDX_PAT          = 1 << 16;
const TUint64 CPUID_FEAT_EDX_PSE36        = 1 << 17;
const TUint64 CPUID_FEAT_EDX_PSN          = 1 << 18;
const TUint64 CPUID_FEAT_EDX_CLF          = 1 << 19;
const TUint64 CPUID_FEAT_EDX_DTES         = 1 << 21;
const TUint64 CPUID_FEAT_EDX_ACPI         = 1 << 22;
const TUint64 CPUID_FEAT_EDX_MMX          = 1 << 23;
const TUint64 CPUID_FEAT_EDX_FXSR         = 1 << 24;
const TUint64 CPUID_FEAT_EDX_SSE          = 1 << 25;
const TUint64 CPUID_FEAT_EDX_SSE2         = 1 << 26;
const TUint64 CPUID_FEAT_EDX_SS           = 1 << 27;
const TUint64 CPUID_FEAT_EDX_HTT          = 1 << 28;
const TUint64 CPUID_FEAT_EDX_TM1          = 1 << 29;
const TUint64 CPUID_FEAT_EDX_IA64         = 1 << 30;
const TUint64 CPUID_FEAT_EDX_PBE          = 1 << 31;

typedef union {
  struct {
    TUint32 ecx;
    TUint32 edx;
  } regs;
  TUint64 features;
  void Dump() {
    dprint("ECX: ");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_SSE3 ? "SSE3" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_PCLMUL ? "PCLMUL" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_DTES64 ? "DTES64" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_MONITOR ? "MONITOR" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_DS_CPL ? "DS_CPL" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_VMX ? "VMX" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_SMX ? "SMX" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_EST ? "EST" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_TM2 ? "TM2" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_SSSE3 ? "SSSE3" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_CID ? "CID" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_FMA ? "FMA" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_CX16 ? "CX16" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_ETPRD ? "ETPRD" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_PDCM ? "PDCM" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_PCIDE ? "PCIDE" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_DCA ? "DCA" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_SSE4_1 ? "SSE4_1" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_SSE4_2 ? "SSE4_2" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_X2APIC ? "X2APIC" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_MOVBE ? "MOVBE" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_POPCNT ? "POPCNT" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_AES ? "AES" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_XSAVE ? "XSAVE" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_OSXSAVE ? "OSXSAVE" : "");
    dprint("%s ", regs.ecx & CPUID_FEAT_ECX_AVX ? "AVX" : "");
    dprint("\n");

    dprint("EDX: ");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_FPU ? "FPU" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_VME ? "VME" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_DE ? "DE" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_PSE ? "PSE" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_TSC ? "TSC" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_MSR ? "MSR" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_PAE ? "PAE" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_MCE ? "MCE" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_CX8 ? "CX8" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_APIC ? "APIC" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_SEP ? "SEP" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_MTRR ? "MTRR" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_PGE ? "PGE" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_MCA ? "MCA" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_CMOV ? "CMOV" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_PAT ? "PAT" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_PSE36 ? "PSE36" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_PSN ? "PSN" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_CLF ? "CLF" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_DTES ? "DTES" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_ACPI ? "ACPI" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_MMX ? "MMX" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_FXSR ? "FXSR" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_SSE ? "SSE" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_SSE2 ? "SSE2" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_SS ? "SS" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_HTT ? "HTT" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_TM1 ? "TM1" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_IA64 ? "IA64" : "");
    dprint("%s ", regs.edx & CPUID_FEAT_EDX_PBE ? "PBE" : "");
    dprint("\n");
  }
} PACKED TCpuFeatures;

extern "C" TCpuFeatures GetCpuFeatures();

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

static __inline void cli() {
  __asm__ __volatile__("cli");
}

static __inline void sti() {
  __asm__ __volatile__("sti");
}

static __attribute__((noreturn)) inline void halt() {
  //  disable_interrupts();
  while (true) {
    __asm__ __volatile__("hlt");
  }
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

// https://wiki.osdev.org/Inline_Assembly/Examples
static inline void outb(TUint16 port, TUint8 val) {
  asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
  /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
   * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
   * The  outb  %al, %dx  encoding is the only option for all other cases.
   * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

static inline void outw(TUint16 port, TUint16 val) {
  asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
  /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
   * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
   * The  outb  %al, %dx  encoding is the only option for all other cases.
   * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

static inline void outl(TUint16 port, TUint32 val) {
  asm volatile ( "outl %0, %1" : : "a"(val), "Nd"(port) );
  /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
   * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
   * The  outb  %al, %dx  encoding is the only option for all other cases.
   * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

static inline TUint8 inb(TUint16 port) {
  TUint8 ret;
  asm volatile ( "inb %1, %0"
		 : "=a"(ret)
		 : "Nd"(port) );
  return ret;
}

static inline TUint16 inw(TUint16 port) {
  TUint16 ret;
  asm volatile ( "inw %1, %0"
		 : "=a"(ret)
		 : "Nd"(port) );
  return ret;
}

static inline TUint32 inl(TUint16 port) {
  TUint32 ret;
  asm volatile ( "inl %1, %0"
		 : "=a"(ret)
		 : "Nd"(port) );
  return ret;
}



#endif
