#ifndef EXEC_MESSAGE_H
#define EXEX_MESSAGE_H

#include <Exec/BBase.h>
#include <Exec/BList.h>

class BMessagePort;

class BMessage : public BNodePri {
public:
  BMessage(BMessagePort *aReplyPort);
  ~BMessage();

public:
  void Reply(BMessagePort *aReplyPort = ENull);

protected:
  BMessagePort *mReplyPort;
};

class BMessageList : public BListPri {
public:
  BMessage *RemHead() { return (BMessage *)BListPri::RemHead(); }

  void AddTail(BMessage *aMessage) { BListPri::AddTail(aMessage); }

  BMessage *First()  { return (BMessage *)mNext; }

  BMessage *Next(BMessage *aCurrent)  { return (BMessage *)aCurrent->mNext; }

  BMessage *Last()  { return (BMessage *)mPrev; }

  BMessage *Prev(BMessage *aCurrent) { return (BMessage *)aCurrent->mPrev; }

  TBool End(BMessage *aCurrent) { return aCurrent == (BMessage *)this; }
};

class BMessagePort : public BBase {
public:
  BMessagePort(TUint64 aSignal);
  ~BMessagePort();

public:
  BMessage *GetMessage();

protected:
  TUint64 mSignal;    // mask of Signal bits to set when message received
};

#endif
