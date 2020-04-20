#ifndef BMESSAGEPORT_H
#define BMESSAGEPORT_H

#include <BTask.h>

class BMessagePort;

class BMessage : public BNode {
public:
  /**
    * if aRpleyPort is ENull, then Message cannot be replied to.
    */
  BMessage(BMessagePort *aReplyPort = ENull);
  ~BMessage();

  void SendMessage(BMessagePort *aToPort);
  void ReplyMessage();

protected:
  BMessagePort *mReplyPort;
};

class BMessageList : public BList {
public:
  BMessageList(const char *aName);
  ~BMessageList();
};

class BMessagePort : public BNodePri {
  friend ExecBase;

public:
  BMessagePort(const char *aName, BTask *aOwner, TInt64 aSignalBit, TInt64 aPri = LIST_PRI_DEFAULT);
  ~BMessagePort();

  void ReceiveMessage(BMessage *aMessage);
  TInt64 SignalNumber() { return mSignalBit; }

protected:
  BTask *mOwner;
  TInt64 mSignalBit;
  BMessageList *mList;
};

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
