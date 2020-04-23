#ifndef KERNEL_INTERRUPTS_H
#define KERNEL_INTERRUPTS_H

// Model/encapsulate the CPU interrupts facilities

#include <Exec/Types.h>
#include <Exec/Memory.h>
#include <x86/bochs.h>
//#include <posix/string.h>

/*******************************************************/

const TInt INTERRUPTS = 256;

// hardware interrupt handler
typedef TBool (TInterruptHandler)(TInt64 aInterruptNumber, void *aData);

typedef struct HANDLER_INFO {
  void set(TInt64 aInterruptNumber, TInterruptHandler *aHandler, void *aData, const char *aDescription) {
    handler = aHandler;
    CopyString(description, aDescription);
    mInterruptNumber = aInterruptNumber;
    data = aData;
  }
  TInt64 mInterruptNumber;
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
};

#endif
