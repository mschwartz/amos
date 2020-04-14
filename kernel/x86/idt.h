#ifndef KERNEL_INTERRUPTS_H
#define KERNEL_INTERRUPTS_H

// Model/encapsulate the CPU interrupts facilities

#include <Exec/BTypes.h>
#include <x86/bochs.h>
#include <posix/string.h>

/*******************************************************/

#if 0
typedef struct registers {
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;
	uint64_t rbp;

	uint64_t rdi; // 16
	uint64_t rsi;
	uint64_t rbx;
	uint64_t rdx; // 36
	uint64_t rcx;
	uint64_t rax;

	uint32_t isr_num; //48
	uint32_t err_code;

	uint64_t rip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t useresp;
	uint32_t ss;
  void Dump() {
    dprint("registers @ %x\n", this);
    dprint(" isr_num: %x\n", isr_num);
    dprint(" err_code: %x\n", err_code);

    dprint(" rax: %x\n", rax);
    dprint(" rbx: %x\n", rbx);
    dprint(" rcx: %x\n", rcx);
    dprint(" rdx: %x\n", rdx);
    dprint(" rsi: %x\n", rsi);
    dprint(" rdi: %x\n", rdi);

    dprint(" rbp: %x\n", rbp);

    dprint(" cs: %x\n", cs);
    dprint(" ds: %x\n", ds);
    dprint(" es: %x\n", es);
    dprint(" fs: %x\n", fs);
    dprint(" gs: %x\n", gs);
  }
} __attribute__((packed)) registers_t;
#endif

const TInt INTERRUPTS = 256;

// hardware interrupt handler
typedef TBool (TInterruptHandler)(void *aData);

typedef struct HANDLER_INFO {
  void set(TInterruptHandler *aHandler, void *aData, const char *aDescription) {
    handler = aHandler;
    strcpy(description, aDescription);
    data = aData;
  }
  TInterruptHandler *handler;
  char description[64];
  TAny *data;
} isr_handler_t;

//extern void init_interrupts();

class IDT {
public:
  IDT();
  ~IDT();

public:
  static const char *interrupt_description(TUint16 n);
public:
  static void disable_interrupts() { asm volatile("cli\n\t"); }
  static void enable_interrupts() { asm volatile("sti\n\t"); }

  static void halt() { asm volatile("cli \n\t"
                             "hlt \n\t"); }

  static void suspend() { asm volatile("hlt \n\t"); }

  static void raise_interrupt(int p) { asm volatile("int %0\n\t" ::"i"(p)); };

public:
  // interrupt handlers return true if the kernel could/should perform a task switch
  static void install_handler(TUint8 aIndex, TInterruptHandler *aHandler, TAny *aData, const char *aDescription = "undefined");

//protected:
//  interrupt_handler_t interrupt_handlers[INTERRUPTS];
};

//extern IDT *gIDT;

#endif
