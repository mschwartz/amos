#ifndef BMESSAGEPORT_H
#define BMESSAGEPORT_H

#include <Exec/BBase.h>
#include <Exec/BList.h>

class BMessagePort;

class BMessage : public BNodePri {
public:
  BMessage(BMessagePort& aReplyPort);
  ~BMessage();

public:
  void Reply(BMessagePort *aReplyPort = ENull);
  void Send(BMessagePort *aMessagePort);

protected:
  BMessagePort& mReplyPort;
};

class BMessageList : public BListPri {
  public:
    BMessageList(const char *aName = "Message List");
public:
  BMessage *RemHead() { return (BMessage *)BListPri::RemHead(); }

  void AddTail(BMessage& aMessage) { BListPri::AddTail(aMessage); }

  BMessage *First()  { return (BMessage *)mNext; }

  BMessage *Next(BMessage *aCurrent)  { return (BMessage *)aCurrent->mNext; }

  BMessage *Last()  { return (BMessage *)mPrev; }

  BMessage *Prev(BMessage *aCurrent) { return (BMessage *)aCurrent->mPrev; }

  TBool End(BMessage *aCurrent) { return aCurrent == (BMessage *)this; }
};

class BMessagePort : public BNode {
public:
  BMessagePort(TUint64 aSignal, const char *aName = "Message Port");
  ~BMessagePort();

public:
  BMessage *GetMessage();
  void SendMessage(BMessage *aMessage);

protected:
  TUint64 mSignal;    // mask of Signal bits to set when message received
  BMessageList mMessageList;
};

#endif
