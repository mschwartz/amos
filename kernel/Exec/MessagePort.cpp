#include <Exec/MessagePort.hpp>
#include <Exec/ExecBase.hpp>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

BMessage::BMessage(MessagePort *aReplyPort) : BNodePri("BMessage") {
  mReplyPort = aReplyPort;
};

BMessage::~BMessage() {
  //
}

void BMessage::Send(MessagePort *aToPort) {
  aToPort->ReceiveMessage(this);
}

void BMessage::Reply() {
  if (mReplyPort) {
    mReplyPort->ReceiveMessage(this);
  }
}

void BMessage::Dump() {
  dlog("BMessage: %x %s\n", mReplyPort, mReplyPort ? mReplyPort->NodeName() : "No Name");
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

MessagePort::MessagePort(const char *aName, BTask *aOwner, TInt64 aSignalBit, TInt64 aPri)
    : BNodePri(aName, aPri) {
  DISABLE;
  mOwner = aOwner;
  mSignalBit = aSignalBit;

  mList = new BMessageList(aName);
  ENABLE;
}

MessagePort::~MessagePort() {
  //
}

BMessage *MessagePort::GetMessage() {
  DISABLE;
  BMessage *m = (BMessage *)mList->RemHead();
  if (m) {
    m->mNext = m->mPrev = ENull;
  }

  ENABLE;
  return m;
}

void MessagePort::ReceiveMessage(BMessage *aMessage) {
  DISABLE;
  mList->AddTail(*aMessage);
  ENABLE;

  mOwner->Signal(1 << mSignalBit);
}

void MessagePort::Dump() {
  dlog("MessagePort: %x %s\n", this, this->NodeName());
  dlog("      mOwner: %s\n", mOwner->NodeName());
  dlog("  mSignalBit: %d\n", mSignalBit);
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

BMessageList::BMessageList(const char *aName) : BListPri(aName) {
  //
}

BMessageList::~BMessageList() {
}

void BMessageList::Dump() {
  TUint64 flags = GetFlags();
  cli();

  for (BMessage *m = (BMessage *)First(); !End(m); m = (BMessage *)m->mNext) {
    m->Dump();
  }

  SetFlags(flags);
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

MessagePortList::MessagePortList(const char *aName) : BListPri(aName) {
  //
}

MessagePortList::~MessagePortList() {
}
