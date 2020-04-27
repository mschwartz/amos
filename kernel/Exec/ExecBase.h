#ifndef EXEC_EXECBASE_H
#define EXEC_EXECBASE_H

#include <Exec/BBase.h>
#include <Exec/BInterrupt.h>
#include <Exec/BTask.h>
#include <Exec/MessagePort.h>
#include <Exec/BDevice.h>
#include <Devices/Screen.h>
#include <x86/gdt.h>
#include <x86/mmu.h>
#include <x86/idt.h>
#include <x86/cpu.h>
#include <Devices/PIC.h>

class TimerDevice;
class RtcDevice;

class ExecBase : public BBase {
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
  GDT *mGDT;
  MMU *mMMU;
  IDT *mIDT;
//  CPU *mCPU;

  //
  // INTERRUPTS
  //
public:
  void AddInterruptHandler(TUint8 aIndex, TInterruptHandler *aHandler, TAny *aData, const char *aDescription = "undefined");

  void SetIntVector(EInterruptNumber aInterruptNumber, BInterrupt *aInterrupt);
  void RemoveIntVector(BInterrupt *aInterrupt);
  //
protected:
  // interrupt lists
  static TBool RootHandler(TInt64 aInterruptNumber, TAny *aData);
  void SetInterrupt(EInterruptNumber aInterruptNumber, const char *aName);
  void SetException(EInterruptNumber aInterruptNumber, const char *aName);
  void SetTrap(EInterruptNumber aInterruptNumber, const char *aName);
  void InitInterrupts();
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

  void Kickstart(); // kickstart multitasking.  Only call once from main() !!!!
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
  void AddMessagePort(MessagePort& aMessagePort);
  TBool RemoveMessagePort(MessagePort& aMessagePort);

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
  TUint64 SystemTicks();

protected:
  TimerDevice *mTimer;
  RtcDevice *mRtc;

};

extern ExecBase gExecBase;

extern "C" TUint64 GetFlags();
extern "C" void SetFlags(TUint64 aFlags);

#endif
