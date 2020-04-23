#ifndef BMESSAGEPORT_H
#define BMESSAGEPORT_H

#include <BTask.h>

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

  void SendMessage(MessagePort *aToPort);
  void ReplyMessage();

public:
  void Dump();

protected:
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
    return (MessagePort *)Find(aName);
  }
};

#endif
