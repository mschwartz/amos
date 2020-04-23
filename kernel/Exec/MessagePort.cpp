#include <Exec/MessagePort.h>
#include <Exec/ExecBase.h>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

BMessage::BMessage(MessagePort *aReplyPort) : BNodePri("BMessage") {
  mReplyPort = aReplyPort;
};

BMessage::~BMessage() {
  //
}

void BMessage::SendMessage(MessagePort *aToPort) {
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

MessagePort::MessagePort(const char *aName, BTask *aOwner, TInt64 aSignalBit, TInt64 aPri)
    : BNodePri(aName, aPri) {
  mOwner = aOwner;
  mSignalBit = aSignalBit;

  mList = new BMessageList(aName);
}

MessagePort::~MessagePort() {
  //
}

BMessage *MessagePort::GetMessage() {
  TUint64 flags = GetFlags();
  cli();

  BMessage *m = (BMessage *)mList->RemHead();

  SetFlags(flags);
  return m;
}

void MessagePort::ReceiveMessage(BMessage *aMessage) {
  TUint64 flags = GetFlags();
  cli();

  mList->AddTail(*aMessage);
  mOwner->Signal(1<<mSignalBit);

  SetFlags(flags);
}

void MessagePort::Dump() {
  dprintf("MessagePort: %x %s\n", this, this->NodeName());
  dprint("      mOwner: %s\n", mOwner->NodeName());
  dprint("  mSignalBit: %d\n", mSignalBit);
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

  for (BMessage *m = (BMessage *)First(); !End(m); m = (BMessage *)m->mNext)  {
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

