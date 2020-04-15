#ifndef EXEC_EXECBASE_H
#define EXEC_EXECBASE_H

#include <Exec/BBase.h>
#include <Exec/BTask.h>
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

protected:
  Screen *mScreen;
  GDT *mGDT;
  MMU *mMMU;
  IDT *mIDT;
  CPU *mCPU;

public:
  void AddTask(BTask *aTask);

protected:
  BTaskList mActiveTasks, mWaitingTasks;

};

extern ExecBase gExecBase;
#endif
