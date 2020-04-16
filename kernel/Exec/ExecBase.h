#ifndef EXEC_EXECBASE_H
#define EXEC_EXECBASE_H

#include <Exec/BBase.h>
#include <Exec/BInterrupt.h>
#include <Exec/BTask.h>
#include <Exec/BDevice.h>
#include <Devices/Screen.h>
#include <x86/gdt.h>
#include <x86/mmu.h>
#include <x86/idt.h>
#include <x86/cpu.h>
#include <Devices/PIC.h>

class ExecBase : public BBase {
public:
  ExecBase();
  ~ExecBase();

public:
  // put character, string to console, wherever console is.
  void putc(char c);
  void puts(const char *s);
  void newline();

public:
  void Hello();
  void AddInterruptHandler(TUint8 aIndex, TInterruptHandler *aHandler, TAny *aData, const char *aDescription = "undefined");

  void GuruMeditation();

protected:
  Screen *mScreen;
  GDT *mGDT;
  MMU *mMMU;
  IDT *mIDT;
  CPU *mCPU;

public:
  TBool SetIntVector(EInterruptNumber aInterruptNumber, BInterrupt *aInterrupt);
  void RemoveIntVector(BInterrupt *aInterrupt);
  //
public:
  void Enable();
  void Disable();

protected:
  TInt64 mDisableNestCount;

protected:
  // interrupt lists
  BInterruptList mInterrupts[EMaxInterrupts];

public:
  void AddTask(BTask *aTask);
  void Reschedule();
  BTask *GetCurrentTask() { return mCurrentTask; }
  /**
    * Put task to sleep until any of its sigwait signals are set.
    */
  void WaitSignal(BTask *aTask);
  void Wait(BTask *aTask);
  void Wake(BTask *aTask);

protected:
  BTaskList mActiveTasks, mWaitingTasks;
  BTask *mCurrentTask;

public:
  void AddDevice(BDevice *aDevice);
  BDevice *FindDevice(const char *aName);

protected:
  BDeviceList mDeviceList;
};

extern ExecBase gExecBase;
#endif
