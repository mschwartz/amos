#ifndef EXEC_TASK_H
#define EXEC_TASK_H

#include <BList.h>

class BTask : public BNodePri {
public:
  BTask();
  virtual ~BTask();
public:
  virtual void Run();
protected:
  TUint64 mSigAlloc;
  TUint64 mSigWait;
  TUint64 mSigReceived;
  TAny *mRegSP, *mUpperSP, *mLowerSP;
};

class BTaskList : public BListPri {
public:
  BTask *RemHead() { return (BTask *)BListPri::RemHead(); }

  void AddTail(BTask *aTask) { BListPri::AddTail((BNodePri *)aTask); }

  BTask *First()  { return (BTask *)mNext; }

  BTask *Next(BTask *aCurrent)  { return (BTask *)aCurrent->mNext; }

  BTask *Last()  { return (BTask *)mPrev; }

  BTask *Prev(BTask *aCurrent) { return (BTask *)aCurrent->mPrev; }

  TBool End(BTask *aCurrent) { return aCurrent == (BTask *)this; }
};

extern "C" BTask *gCurrentTask;

#endif
