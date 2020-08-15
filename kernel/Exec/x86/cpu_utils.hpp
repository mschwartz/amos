#ifndef AMOS_EXEC_X86_CPU_UTILS_H
#define AMOS_EXEC_X86_CPU_UTILS_H

#include <Types.hpp>

extern "C" void wrmsr(TUint64 aRegister, TAny *aValue);
extern "C" TUint64 rdmsr(TUint64 aRegister);

/**
 * MSR registers
 */
const TUint32 MSR_IA32_VMX_BASIC = 0x480;                            // Basic VMX information.
const TUint32 MSR_IA32_VMX_PINBASED_CTLS = 0x481;                    // Allowed settings fr pin-based VM execution controls.
const TUint32 MSR_IA32_VMX_PROCBASED_CTLS = 0x482;                   // Allowed settings for proc-based VM execution controls.
const TUint32 MSR_IA32_VMX_EXIT_CTLS = 0x483;                        // Allowed settings for the VM-exit controls.
const TUint32 MSR_IA32_VMX_ENTRY_CTLS = 0x484;                       // Allowed settings for the VM-entry controls.
const TUint32 MSR_IA32_VMX_MISC = 0x485;                             // Misc VMX info.
const TUint32 MSR_IA32_VMX_CR0_FIXED0 = 0x486;                       // Fixed cleared bits in CR0.
const TUint32 MSR_IA32_VMX_CR0_FIXED1 = 0x487;                       // Fixed set bits in CR0.
const TUint32 MSR_IA32_VMX_CR4_FIXED0 = 0x488;                       // Fixed cleared bits in CR4.
const TUint32 MSR_IA32_VMX_CR4_FIXED1 = 0x489;                       // Fixed set bits in CR4.
const TUint32 MSR_IA32_VMX_VMCS_ENUM = 0x48A;                        // Information for enumerating fields in the VMCS.
const TUint32 MSR_IA32_VMX_PROCBASED_CTLS2 = 0x48B;                  // Allowed settings for secondary proc-based VM execution controls
const TUint32 MSR_IA32_VMX_EPT_VPID_CAP = 0x48C;                     // EPT capabilities.
const TUint32 MSR_IA32_VMX_TRUE_PINBASED_CTLS = 0x48D;               // Allowed settings of all pin-based VM execution controls.
const TUint32 MSR_IA32_VMX_TRUE_PROCBASED_CTLS = 0x48E;              // Allowed settings of all proc-based VM execution controls.
const TUint32 MSR_IA32_VMX_TRUE_EXIT_CTLS = 0x48F;                   // Allowed settings of all VMX exit controls.
const TUint32 MSR_IA32_VMX_TRUE_ENTRY_CTLS = 0x490;                  // Allowed settings of all VMX entry controls.
const TUint32 MSR_IA32_VMX_VMFUNC = 0x491;                           // Allowed settings for the VM-function controls.
const TUint32 MSR_IA32_RTIT_CTL = 0x570;                             // Intel PT - Enable and control for trace packet generation.
const TUint32 MSR_IA32_DS_AREA = 0x600;                              // DS Save Area (R/W).
const TUint32 MSR_RAPL_POWER_UNIT = 0x606;                           // Running Average Power Limit (RAPL) power units.
const TUint32 MSR_PKGC3_IRTL = 0x60a;                                // Package C3 Interrupt Response Limit.
const TUint32 MSR_PKGC_IRTL1 = 0x60b;                                // Package C6/C7S Interrupt Response Limit 1.
const TUint32 MSR_PKGC_IRTL2 = 0x60c;                                // Package C6/C7S Interrupt Response Limit 2.
const TUint32 MSR_PKG_C2_RESIDENCY = 0x60d;                          // Package C2 Residency Counter.
const TUint32 MSR_PKG_POWER_LIMIT = 0x610;                           // PKG RAPL Power Limit Control.
const TUint32 MSR_PKG_ENERGY_STATUS = 0x611;                         // PKG Energy Status.
const TUint32 MSR_PKG_PERF_STATUS = 0x613;                           // PKG Perf Status.
const TUint32 MSR_PKG_POWER_INFO = 0x614;                            // PKG RAPL Parameters.
const TUint32 MSR_DRAM_POWER_LIMIT = 0x618;                          // DRAM RAPL Power Limit Control.
const TUint32 MSR_DRAM_ENERGY_STATUS = 0x619;                        // DRAM Energy Status.
const TUint32 MSR_DRAM_PERF_STATUS = 0x61b;                          // DRAM Performance Throttling Status.
const TUint32 MSR_DRAM_POWER_INFO = 0x61c;                           // DRAM RAPL Parameters.
const TUint32 MSR_PKG_C10_RESIDENCY = 0x632;                         // Package C10 Residency Counter.
const TUint32 MSR_PP0_ENERGY_STATUS = 0x639;                         // PP0 Energy Status.
const TUint32 MSR_PP1_ENERGY_STATUS = 0x641;                         // PP1 Energy Status.
const TUint32 MSR_TURBO_ACTIVATION_RATIO = 0x64c;                    // Turbo Activation Ratio.
const TUint32 MSR_CORE_PERF_LIMIT_REASONS = 0x64f;                   // Core Performance Limit Reasons.
const TUint32 MSR_IA32_X2APIC_START = 0x800;                         // X2APIC MSR range start.
const TUint32 MSR_IA32_X2APIC_ID = 0x802;                            // X2APIC MSR -  APIC ID Register.
const TUint32 MSR_IA32_X2APIC_VERSION = 0x803;                       // X2APIC MSR -  APIC Version Register.
const TUint32 MSR_IA32_X2APIC_TPR = 0x808;                           // X2APIC MSR -  Task Priority Register.
const TUint32 MSR_IA32_X2APIC_PPR = 0x80A;                           // X2APIC MSR -  Processor Priority register.
const TUint32 MSR_IA32_X2APIC_EOI = 0x80B;                           // X2APIC MSR -  End Of Interrupt register.
const TUint32 MSR_IA32_X2APIC_LDR = 0x80D;                           // X2APIC MSR -  Logical Destination Register.
const TUint32 MSR_IA32_X2APIC_SVR = 0x80F;                           // X2APIC MSR -  Spurious Interrupt Vector Register.
const TUint32 MSR_IA32_X2APIC_ISR0 = 0x810;                          // X2APIC MSR -  In-service Register (bits 31:0).
const TUint32 MSR_IA32_X2APIC_ISR1 = 0x811;                          // X2APIC MSR -  In-service Register (bits 63:32).
const TUint32 MSR_IA32_X2APIC_ISR2 = 0x812;                          // X2APIC MSR -  In-service Register (bits 95:64).
const TUint32 MSR_IA32_X2APIC_ISR3 = 0x813;                          // X2APIC MSR -  In-service Register (bits 127:96).
const TUint32 MSR_IA32_X2APIC_ISR4 = 0x814;                          // X2APIC MSR -  In-service Register (bits 159:128).
const TUint32 MSR_IA32_X2APIC_ISR5 = 0x815;                          // X2APIC MSR -  In-service Register (bits 191:160).
const TUint32 MSR_IA32_X2APIC_ISR6 = 0x816;                          // X2APIC MSR -  In-service Register (bits 223:192).
const TUint32 MSR_IA32_X2APIC_ISR7 = 0x817;                          // X2APIC MSR -  In-service Register (bits 255:224).
const TUint32 MSR_IA32_X2APIC_TMR0 = 0x818;                          // X2APIC MSR -  Trigger Mode Register (bits 31:0).
const TUint32 MSR_IA32_X2APIC_TMR1 = 0x819;                          // X2APIC MSR -  Trigger Mode Register (bits 63:32).
const TUint32 MSR_IA32_X2APIC_TMR2 = 0x81A;                          // X2APIC MSR -  Trigger Mode Register (bits 95:64).
const TUint32 MSR_IA32_X2APIC_TMR3 = 0x81B;                          // X2APIC MSR -  Trigger Mode Register (bits 127:96).
const TUint32 MSR_IA32_X2APIC_TMR4 = 0x81C;                          // X2APIC MSR -  Trigger Mode Register (bits 159:128).
const TUint32 MSR_IA32_X2APIC_TMR5 = 0x81D;                          // X2APIC MSR -  Trigger Mode Register (bits 191:160).
const TUint32 MSR_IA32_X2APIC_TMR6 = 0x81E;                          // X2APIC MSR -  Trigger Mode Register (bits 223:192).
const TUint32 MSR_IA32_X2APIC_TMR7 = 0x81F;                          // X2APIC MSR -  Trigger Mode Register (bits 255:224).
const TUint32 MSR_IA32_X2APIC_IRR0 = 0x820;                          // X2APIC MSR -  Interrupt Request Register (bits 31:0).
const TUint32 MSR_IA32_X2APIC_IRR1 = 0x821;                          // X2APIC MSR -  Interrupt Request Register (bits 63:32).
const TUint32 MSR_IA32_X2APIC_IRR2 = 0x822;                          // X2APIC MSR -  Interrupt Request Register (bits 95:64).
const TUint32 MSR_IA32_X2APIC_IRR3 = 0x823;                          // X2APIC MSR -  Interrupt Request Register (bits 127:96).
const TUint32 MSR_IA32_X2APIC_IRR4 = 0x824;                          // X2APIC MSR -  Interrupt Request Register (bits 159:128).
const TUint32 MSR_IA32_X2APIC_IRR5 = 0x825;                          // X2APIC MSR -  Interrupt Request Register (bits 191:160).
const TUint32 MSR_IA32_X2APIC_IRR6 = 0x826;                          // X2APIC MSR -  Interrupt Request Register (bits 223:192).
const TUint32 MSR_IA32_X2APIC_IRR7 = 0x827;                          // X2APIC MSR -  Interrupt Request Register (bits 255:224).
const TUint32 MSR_IA32_X2APIC_ESR = 0x828;                           // X2APIC MSR -  Error Status Register.
const TUint32 MSR_IA32_X2APIC_LVT_CMCI = 0x82F;                      // X2APIC MSR - LVT CMCI Register.
const TUint32 MSR_IA32_X2APIC_ICR = 0x830;                           // X2APIC MSR -  Interrupt Command Register.
const TUint32 MSR_IA32_X2APIC_LVT_TIMER = 0x832;                     // X2APIC MSR -  LVT Timer Register.
const TUint32 MSR_IA32_X2APIC_LVT_THERMAL = 0x833;                   // X2APIC MSR -  LVT Thermal Sensor Register.
const TUint32 MSR_IA32_X2APIC_LVT_PERF = 0x834;                      // X2APIC MSR -  LVT Performance Counter Register.
const TUint32 MSR_IA32_X2APIC_LVT_LINT0 = 0x835;                     // X2APIC MSR -  LVT LINT0 Register.
const TUint32 MSR_IA32_X2APIC_LVT_LINT1 = 0x836;                     // X2APIC MSR -  LVT LINT1 Register.
const TUint32 MSR_IA32_X2APIC_LVT_ERROR = 0x837;                     // X2APIC MSR -  LVT Error Register .
const TUint32 MSR_IA32_X2APIC_TIMER_ICR = 0x838;                     // X2APIC MSR -  Timer Initial Count Register.
const TUint32 MSR_IA32_X2APIC_TIMER_CCR = 0x839;                     // X2APIC MSR -  Timer Current Count Register.
const TUint32 MSR_IA32_X2APIC_TIMER_DCR = 0x83E;                     // X2APIC MSR -  Timer Divide Configuration Register.
const TUint32 MSR_IA32_X2APIC_SELF_IPI = 0x83F;                      // X2APIC MSR - Self IPI.
const TUint32 MSR_IA32_X2APIC_END = 0x8FF;                           // X2APIC MSR range end.
const TUint32 MSR_IA32_X2APIC_LVT_START = MSR_IA32_X2APIC_LVT_TIMER; // X2APIC MSR - LVT start range.
const TUint32 MSR_IA32_X2APIC_LVT_END = MSR_IA32_X2APIC_LVT_ERROR;   // X2APIC MSR - LVT end range (inclusive).

// TODO: move I/O to a separate file
extern "C" void
cli();
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
