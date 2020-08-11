#ifndef KERNEL_INTERRUPTS_H
#define KERNEL_INTERRUPTS_H

// Model/encapsulate the CPU interrupts facilities

#include <Types.hpp>
#include <Exec/Memory.hpp>

/*******************************************************/

const TInt INTERRUPTS = 256;

// hardware interrupt handler
typedef TBool(TInterruptHandler)(TInt64 aInterruptNumber, void *aData);

typedef struct HANDLER_INFO {
  void Set(TInt64 aInterruptNumber, TInterruptHandler *aHandler, void *aData, const char *aDescription) {
    mHandler = aHandler;
    CopyString(mDescription, aDescription);
    mInterruptNumber = aInterruptNumber;
    mData = aData;
  }
  //
  TInt64 mInterruptNumber;
  TInterruptHandler *mHandler;
  char mDescription[64];
  TAny *mData;
} TIsrHandler;

//extern void init_interrupts();

class IDT {
public:
  IDT();
  ~IDT();

  TBool Alive() { return mAlive; }

  void Install();
  
public:
  static const char *InterruptDescription(TUint16 n);

public:
  static void DisableInterrupts() { asm volatile("cli\n\t"); }
  static void EnableInterrupts() { asm volatile("sti\n\t"); }

  static void HaltAndCatchFire() { asm volatile("cli \n\t"
                                                "hlt \n\t"); }

  static void Suspend() { asm volatile("hlt \n\t"); }
  static void Halt() { asm volatile("hlt \n\t"); }

  static void RaiseInterrupt(int p) { asm volatile("int %0\n\t" ::"i"(p)); };

public:
  // interrupt handlers return true if the kernel could/should perform a task switch
  static void InstallHandler(TUint8 aIndex, TInterruptHandler *aHandler, TAny *aData, const char *aDescription = "undefined");

protected:
  TBool mAlive;
};

#endif
