#ifndef BMESSAGEPORT_H
#define BMESSAGEPORT_H

#include <Exec/BTask.hpp>
#include <Exec/SpinLock.hpp>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class MessagePort;

class BMessage : public BNodePri {
  friend MessagePort;

public:
  /**
    * if aRpleyPort is ENull, then Message cannot be replied to.
    */
  BMessage(MessagePort *aReplyPort = ENull);
  ~BMessage();

  void Send(MessagePort *aToPort);
  void Reply();

public:
  void Dump();

  MessagePort *mReplyPort;
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BMessageList : public BListPri {
public:
  BMessageList(const char *aName);
  ~BMessageList();

public:
  void Lock() { mSpinLock.Acquire(); }
  void Unlock() { mSpinLock.Release(); }

protected:
  SpinLock mSpinLock;

public:
  void Dump();
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class MessagePort : public BNodePri {
  friend ExecBase;
  friend BMessage;

public:
  MessagePort(const char *aName, BTask *aOwner, TInt64 aSignalBit, TInt64 aPri = LIST_PRI_DEFAULT);
  ~MessagePort();

  TInt64 SignalNumber() { return mSignalBit; }

  BMessage *GetMessage();

public:
  void Dump();

protected:
  void ReceiveMessage(BMessage *aMessage);

public:
  void Lock() { mSpinLock.Acquire(); }
  void Unlock() { mSpinLock.Release(); }

protected:
  SpinLock mSpinLock;

protected:
  BTask *mOwner;
  TInt64 mSignalBit;
  BMessageList *mList;
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class MessagePortList : public BListPri {
public:
  MessagePortList(const char *aName);
  ~MessagePortList();

public:
  MessagePort *FindPort(const char *aName) {

    Lock();
    MessagePort *p = (MessagePort *)Find(aName);
    Unlock();
    return p;
  }

protected:
  SpinLock mSpinLock;

public:
  void Lock() { mSpinLock.Acquire(); }
  void Unlock() { mSpinLock.Release(); }
};

#endif
