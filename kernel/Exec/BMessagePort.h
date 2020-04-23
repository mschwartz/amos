#ifndef BMESSAGEPORT_H
#define BMESSAGEPORT_H

#include <BTask.h>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BMessagePort;

class BMessage : public BNodePri {
  friend BMessagePort;

public:
  /**
    * if aRpleyPort is ENull, then Message cannot be replied to.
    */
  BMessage(BMessagePort *aReplyPort = ENull);
  ~BMessage();

  void SendMessage(BMessagePort *aToPort);
  void ReplyMessage();

public:
  void Dump();

protected:
  BMessagePort *mReplyPort;
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

class BMessagePort : public BNodePri {
  friend ExecBase;
  friend BMessage;

public:
  BMessagePort(const char *aName, BTask *aOwner, TInt64 aSignalBit, TInt64 aPri = LIST_PRI_DEFAULT);
  ~BMessagePort();

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

class BMessagePortList : public BListPri {
public:
  BMessagePortList(const char *aName);
  ~BMessagePortList();

public:
  BMessagePort *FindPort(const char *aName) {
    return (BMessagePort *)Find(aName);
  }
};

#endif
