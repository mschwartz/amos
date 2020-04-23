#ifndef EXEC_TASK_H
#define EXEC_TASK_H

#include <Exec/BList.h>
#include <x86/tasking.h>

const TUint64 default_task_stack_size = 2 * 1024 * 1024;
//const TUint64 default_task_stack_size = 64 * 1024;

class BMessagePort;

class ExecBase;

enum ETaskState {
  ETaskRunning,
  ETaskWaiting,
};

class BTask : public BNodePri {
  friend ExecBase;

public:
  BTask(const char *aName, TInt64 aPri = 0, TUint64 aStackSize = default_task_stack_size);
  virtual ~BTask();

public:
  TTaskRegisters mRegisters;

protected:
  volatile TAny *mUpperSP, *mLowerSP; 
  volatile ETaskState mTaskState;

public:
  virtual void Run() = 0;
  void DumpRegisters(TTaskRegisters *regs);
  void Dump();

protected:
  static void RunWrapper(BTask *aTask);

protected:
  volatile TUint64 mSigAlloc;
  volatile TUint64 mSigWait;
  volatile TUint64 mSigReceived;

protected:
  /**
    * Allocate signals
    *
    * if aSignalMask is -1, allocates next available one
    * returns signal number - it must be shifted to make a bit mask
    */
  TInt8 AllocSignal(TInt64 aSignalNum);
  TBool FreeSignal(TInt64 aSignalNum);
  /**
    * Wait for one or more signals, specified in the signal set (set of signal bits to wait on).
    *
    * Returns a mask of the signals actually received.
    */
  TUint64 Wait(TUint64 aSignalSet);

public:
  /**
    * Receive a signal
    */
  void Signal(TInt64 aSignalBit);

protected:
  BMessagePort *CreateMessagePort(const char *aName = ENull, TInt64 aPri = LIST_PRI_DEFAULT);
  void FreeMessagePort(BMessagePort *aMessagePort);
  TUint64 WaitPort(BMessagePort *aMessagePort);

  /**
    * Wait for some number of seconds.
    */
  void Sleep(TUint64 aSeconds);

protected:
  void Disable();
  void Enable();

  void Forbid();
  void Permit();

protected:
  volatile TInt64 mForbidNestCount, mDisableNestCount;
};

class BTaskList : public BListPri {
public:
  BTaskList() : BListPri("Task List") {}

  static void DumpRegisters(TTaskRegisters *aRegisters);
  void Dump();

public:
  BTask *RemHead() { return (BTask *)BListPri::RemHead(); }

  void Add(BTask &aTask) { BListPri::Add(aTask); }

  //  void AddTail(BTask &aTask) { BListPri::AddTail(aTask); }

  BTask *First() { return (BTask *)mNext; }

  BTask *Next(BTask *aCurrent) { return (BTask *)aCurrent->mNext; }

  BTask *Last() { return (BTask *)mPrev; }

  BTask *Prev(BTask *aCurrent) { return (BTask *)aCurrent->mPrev; }

  TBool End(BTask *aCurrent) { return BListPri::End(aCurrent); }
};

#endif
