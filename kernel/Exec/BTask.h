#ifndef EXEC_TASK_H
#define EXEC_TASK_H

#include <Exec/BList.h>
#include <x86/tasking.h>

class BTask : public BNodePri {
public:
  BTask(const char *aName, TInt64 aPri = 0, TUint64 aStackSize = 2 * 1024 * 1024);
  virtual ~BTask();

public:
  virtual void Run() = 0;
  static void DumpRegisters(task_t *regs);
  void Dump();

public:
  task_t mTaskX64;

protected:
  static void RunWrapper(BTask *aTask);

protected:
  TUint64 mSigAlloc;
  TUint64 mSigWait;
  TUint64 mSigReceived;
  TAny *mUpperSP, *mLowerSP;
};

class BTaskList : public BListPri {
public:
  BTaskList() : BListPri("Task List") {}

  static void DumpRegisters(task_t *aRegisters);
  void Dump();
public:
  BTask *RemHead() { return (BTask *)BListPri::RemHead(); }

  void Add(BTask& aTask) { BListPri::Add(aTask); }

//  void AddTail(BTask &aTask) { BListPri::AddTail(aTask); }

  BTask *First() { return (BTask *)mNext; }

  BTask *Next(BTask *aCurrent) { return (BTask *)aCurrent->mNext; }

  BTask *Last() { return (BTask *)mPrev; }

  BTask *Prev(BTask *aCurrent) { return (BTask *)aCurrent->mPrev; }

  TBool End(BTask *aCurrent) { return aCurrent == (BTask *)this; }
};

#endif
