#ifndef EXEC_BINTERRUPT_H
#define EXEC_BINTERRUPT_H

#include <Exec/BList.h>

enum EInterruptNumber {
  ETimerInterrupt,
  EKeyboardInterrupt,
  EMaxInterrupts,
};

/*
 * To install an interrupt handler, inherit from BInterrupt, create an instance of your class, and call
 * gExecBase.SetIntVector() with it.
 */
class BInterrupt : public BNodePri {
public:
  BInterrupt(const char *aNodeName, TInt64 aPri);
  ~BInterrupt();

public:
  /**
      * You must imeplement a Run() method in your inherited class.  It returns ETrue if it handled the interrupt
      * and no more interrupt handlers in the chain are to be executed.
      */
  virtual TBool Run() = 0;
};

class BInterruptList : public BListPri {
public:
  BInterruptList() : BListPri("Interrupt List") {}
  ~BInterruptList() {}
};

#endif