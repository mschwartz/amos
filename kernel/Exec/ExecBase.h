#ifndef EXEC_EXECBASE_H
#define EXEC_EXECBASE_H

#include <Exec/BBase.h>
#include <Exec/BInterrupt.h>
#include <Exec/BTask.h>
#include <Exec/MessagePort.h>
#include <Exec/BDevice.h>

#include <Devices/Screen.h>
//#include <Exec/x86/cpu.h>

class RtcDevice;
class GDT;
class MMU;
class IDT;
class PIC;
class PS2;

/* External interrupts */
#define IRQ_OFFSET 0x20 /* Interrupt offset for external interrupts */
enum {
  IRQ_TIMER = IRQ_OFFSET, // 0
  IRQ_KEYBOARD,           // 1
  IRQ_SLAVE_PIC,          // 2
  IRQ_COM2,               // 3
  IRQ_COM1,               // 4
  IRQ_LPT2,               // 5
  IRQ_FLOPPY,             // 6
  IRQ_LPT1,               // 7
  IRQ_RTC,                // 8
  IRQ_MASTER_PIC,         // 9
  IRQ_RESERVED1,          // 10
  IRQ_RESERVED2,          // 11
  IRQ_MOUSE,              // 12
  IRQ_COPROCESSOR,        // 13
  IRQ_HARD_DISK,          // 14
  IRQ_RESERVED4,          // 15

  HW_INTERRUPTS
};

typedef struct {
  const char *mVersion;
  TInt32 mScreenWidth;
  TInt32 mScreenHeight;
  TInt32 mScreenDepth;
  TInt32 mScreenPitch;
  TAny *mScreenFrameBuffer;
  TUint64 mMillis;
  void Dump() {
    dprintf("TSystemInfo(%x)\n", this);
    dprintf("mScreenWidth %d\n", mScreenWidth);
    dprintf("mScreenHeight %d\n", mScreenHeight);
    dprintf("mScreenDepth %d\n", mScreenDepth);
    dprintf("mScreenPitch %d\n", mScreenPitch);
    dprintf("mScreenFrameBuffer %x\n", mScreenFrameBuffer);
  }
} TSystemInfo;

class ExecBase : public BBase {
  friend RtcDevice;

protected:
  void Tick() { mSystemInfo.mMillis++; }

public:
  ExecBase();
  ~ExecBase();

  /**
    * Return quantum (timer interrupt frequency for multitasking preemption) in hz
    */
  TInt Quantum() { return 100; }

public:
  // put character, string to console, wherever console is.
  void putc(char c);
  void puts(const char *s);
  void newline();

public:
  void GuruMeditation(const char *aFormat, ...);

#ifdef KGFX
  ScreenVesa *GetScreen() { return mScreen; }
#else
  ScreenVGA *GetScreen() { return mScreen; }
#endif
protected:
#ifdef KGFX
  ScreenVesa *mScreen;
#else
  ScreenVGA *mScreen;
#endif

protected:
  GDT *mGDT;
  MMU *mMMU;
  IDT *mIDT;
  PIC *mPIC;

public:
  PS2 *GetPS2() { return mPS2; }

protected:
  PS2 *mPS2;

  //
  // INTERRUPTS
  //
public:
  void SetIntVector(EInterruptNumber aInterruptNumber, BInterrupt *aInterrupt);
  void RemoveIntVector(BInterrupt *aInterrupt);

  void EnableIRQ(TUint16 aIRQ);
  void DisableIRQ(TUint16 aIRQ);
  void AckIRQ(TUint16 aIRQ);

  //
protected:
  // interrupt lists
  static TBool RootHandler(TInt64 aInterruptNumber, TAny *aData);
  void SetInterrupt(EInterruptNumber aInterruptNumber, const char *aName);
  void SetException(EInterruptNumber aInterruptNumber, const char *aName);
  void SetTrap(EInterruptNumber aInterruptNumber, const char *aName);
  void InitInterrupts();
  //
  BInterruptList mInterrupts[EMaxInterrupts];

public:
  void Enable();
  void Disable();

protected:
  volatile TInt64 mDisableNestCount;

  //
  // TASKS
  //
public:
  void AddTask(BTask *aTask);
  void DumpTasks();
  BTask *GetCurrentTask() { return mCurrentTask; }
  /**
    * Put task to sleep until any of its sigwait signals are set.
    */
  void WaitSignal(BTask *aTask);
  //  void Wait(BTask *aTask);
  void Wake(BTask *aTask);

  void Kickstart();     // kickstart multitasking.  Only call once from main() !!!!
                        //  void Reschedule();
  void RescheduleIRQ(); // from IRQ context
  void Schedule();

protected:
  BTaskList mActiveTasks, mWaitingTasks;
  BTask *mCurrentTask;

  //
  // Message Ports
  //
public:
  void AddMessagePort(MessagePort &aMessagePort);
  TBool RemoveMessagePort(MessagePort &aMessagePort);

  MessagePort *FindMessagePort(const char *aPortName);

protected:
  MessagePortList *mMessagePortList;

  //
  // DEVICES
  //
public:
  void AddDevice(BDevice *aDevice);
  BDevice *FindDevice(const char *aName);

protected:
  BDeviceList mDeviceList;

public:
  TUint64 SystemTicks() { return mSystemInfo.mMillis; }

protected:
  TSystemInfo mSystemInfo;

public:
  void GetSystemInfo(TSystemInfo *aSystemInfo) {
    *aSystemInfo = mSystemInfo;
  }
};

extern ExecBase gExecBase;

extern "C" TUint64 GetFlags();
extern "C" void SetFlags(TUint64 aFlags);

#endif
