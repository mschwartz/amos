#ifndef EXEC_TASK_H
#define EXEC_TASK_H

#include <Types/BList.hpp>
#include <Exec/x86/tasking.hpp>

const TInt64 TASK_PRI_MIN = LIST_PRI_MIN;
const TInt64 TASK_PRI_MAX = LIST_PRI_MAX;
const TInt64 TASK_PRI_DEFAULT = LIST_PRI_DEFAULT;
const TInt64 TASK_PRI_NICE = TASK_PRI_DEFAULT + 1;
const TInt64 TASK_PRI_URGENT = TASK_PRI_DEFAULT - 1;

const TUint64 default_task_stack_size = 2 * 1024 * 1024;

class MessagePort;

class ExecBase;
class Semaphore;
class InspirationBase;
class CPU;

enum ETaskState {
  ETaskRunning,
  ETaskWaiting,
  ETaskBlocked,
};

class BTask : public BNodePri {
  friend ExecBase;
  friend Semaphore;
  friend CPU;

public:
  BTask(const char *aName, TInt64 aPri = 0, TUint64 aStackSize = default_task_stack_size);
  virtual ~BTask();

public:
  TTaskRegisters mRegisters;

public:
  TInt64 ProcessorId();
  CPU *CurrentCPU();

protected:
  volatile ETaskState mTaskState;
  volatile CPU *mCpu;

public:
  /**
   * Run()
   * This is the task's entrypoint.  You must override this; the class is abstract.
   * Return to exit the task, or call Suicide/Exit.  
   * The return value is an arbitrary (for now) exit code.
   */
  virtual TInt64 Run() = 0;
  void Suicide(TInt64 aCode);
  void Exit(TInt64 aCode) {
    Suicide(aCode);
  }

public:
  const char *TaskName() { return mNodeName; }
  void DumpRegisters(TTaskRegisters *regs);
  void Dump();

protected:
  static void RunWrapper(BTask *aTask);

protected:
  Mutex mSignalMutex;
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
  TUint64 Wait(TUint64 aSignalSet = 0);

public:
  /**
    * Receive a signal
    */
  void Signal(TInt64 aSignalBit);

public:
  MessagePort *CreatePort(const char *aName = ENull, TInt64 aPri = LIST_PRI_DEFAULT);

protected:
  void FreePort(MessagePort *aMessagePort);

  /**
   * Wait for message port to be created.
   */
  MessagePort *WaitForPort(const char *aName);

  /**
   * Wait for message port signal bit as well as any optional other signal bits.
   * returns mask of signals received.
   */
  TUint64 WaitPort(MessagePort *aMessagePort, TUint64 aSignalMask = 0);

  /**
   * Wait for two message ports' signal bits as well as any optional other signal bits.
   * returns mask of signals received.
   */
  TUint64 WaitPort(MessagePort *aMessagePort, MessagePort *aOtherPort, TUint64 aSignalMask = 0);

  /**
   * Wait for three message ports' signal bits as well as any optional other signal bits.
   * returns mask of signals received.
   */
  TUint64 WaitPort(MessagePort *aMessagePort, MessagePort *aOtherPort, MessagePort *aOtherOtherPort, TUint64 aSignalMask = 0);

  /**
   * Wait for arbitrary number of  message ports as well as any optional other signal bits .
   * After aSigMask, some number of MessagePorts, followed by ENull.
   * returns mask of signals received.
   */
  TUint64 WaitPorts(TUint64 aSigMask, ...);

public:
  /**
    * Wait for some number of seconds.
    */
  void Sleep(TUint64 aSeconds);
  void MilliSleep(TUint64 aMilliSeconds);

#if 0
protected:
  void Disable();
  void Enable();

  void Forbid();
  void Permit();

protected:
  volatile TInt64 mForbidNestCount, mDisableNestCount;
#endif

protected:
  InspirationBase &mInspirationBase;
};

/**
 *
 */
class BTaskList : public BListPri {
public:
  BTaskList(const char *aName = "Task List") : BListPri(aName) {
    //    dlog("Construct BTaskList %s\n", aName);
  }

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
