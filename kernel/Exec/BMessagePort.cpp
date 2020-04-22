#include <BMessagePort.h>
#include <ExecBase.h>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

BMessage::BMessage(BMessagePort *aReplyPort) : BNodePri("BMessage") {
  mReplyPort = aReplyPort;
};

BMessage::~BMessage() {
  //
}

void BMessage::SendMessage(BMessagePort *aToPort) {
  aToPort->ReceiveMessage(this);
}

void BMessage::ReplyMessage() {
  if (mReplyPort) {
    mReplyPort->ReceiveMessage(this);
  }
}

void BMessage::Dump() {
  dprint("BMessage: %x %s\n", mReplyPort, mReplyPort ? mReplyPort->NodeName() : "No Name");
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

BMessagePort::BMessagePort(const char *aName, BTask *aOwner, TInt64 aSignalBit, TInt64 aPri)
    : BNodePri(aName, aPri) {
  mOwner = aOwner;
  mSignalBit = aSignalBit;

  mList = new BMessageList(aName);
}

BMessagePort::~BMessagePort() {
  //
}

BMessage *BMessagePort::GetMessage() {
  TUint64 flags = GetFlags();
  cli();
  BMessage *m = (BMessage *)mList->RemHead();
  SetFlags(flags);
  return m;
}

void BMessagePort::ReceiveMessage(BMessage *aMessage) {
  TUint64 flags = GetFlags();
  cli();
  mList->AddTail(*aMessage);
  mOwner->Signal(1<<mSignalBit);
  SetFlags(flags);
}

void BMessagePort::Dump() {
  dprintf("BMessagePort: %x %s\n", this, this->NodeName());
  dprint("      mOwner: %s\n", mOwner->NodeName());
  dprint("  mSignalBit: %d\n", mSignalBit);
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

BMessageList::BMessageList(const char *aName) : BList(aName) {
  //
}

BMessageList::~BMessageList() {
}

void BMessageList::Dump() {
  TUint64 flags = GetFlags();
  cli();

  for (BMessage *m = (BMessage *)First(); !End(m); m = (BMessage *)m->mNext)  {
    m->Dump();
  }
 
  SetFlags(flags);
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

BMessagePortList::BMessagePortList(const char *aName) : BListPri(aName) {
  //
}

BMessagePortList::~BMessagePortList() {
}

