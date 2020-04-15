#ifndef EXEC_EXECBASE_H
#define EXEC_EXECBASE_H

#include <Exec/BBase.h>
#include <Exec/BList.h>
#include <Exec/BTask.h>
#include <Exec/BDevice.h>

#include <x86/idt.h>

class GDT;
//class IDT;
class MMU;
class CPU;
class PIC;
class Timer;
class Keyboard;
class Screen;


class ExecBase : public BBase {
public:
  ExecBase();
  ~ExecBase();
  void Init();

public:
  void InstallIrqHandler(TUint8 aIndex, TInterruptHandler *aHandler, TAny *aData, const char *aDescription = "undefined");
  BTask *GetCurrentTask() { 
    return mCurrentTask; 
  }

  void AddDevice(BDevice *aDevice);
  BDevice *FindDevice(const char *aName);

  void SetCurrentTask(BTask *aTask);
  void AddTask(BTask *aTask);
  static void DumpCurrentTask();
  static void DumpCurrentTaskRegisters();

protected:
  BTaskList mTasksReady;
  BTaskList mTasksWait;
  BTask *mCurrentTask;
  BDeviceList *mDeviceList;

protected:
  volatile TInt32 mDisableNestCount;

public:
  void Disable();
  void Enable();

public:
  Screen &GetScreen() { return *mScreen; }
  static ExecBase &GetExecBase();

protected:
  IDT *mIdt;
  GDT *mGdt;
  MMU *mMmu;
  CPU *mCpu;
  PIC *mPic;
  Timer *mTimer;
  Keyboard *mKeyboard;
  Screen *mScreen;
};

extern ExecBase *gExecBase;

#endif
