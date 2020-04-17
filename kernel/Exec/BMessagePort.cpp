#include <BMessagePort.h>
#include <ExecBase.h>

BMessage::BMessage(BMessagePort *aReplyPort) : BNode("BMessage") {
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

BMessagePort::BMessagePort(const char *aName, BTask *aOwner, TInt64 aSignalBit, TInt64 aPri)
    : BNodePri(aName, aPri) {
  mOwner = aOwner;
  mSignalBit = aSignalBit;

  mList = new BMessageList(aName);
}

BMessagePort::~BMessagePort() {
  //
}

void BMessagePort::ReceiveMessage(BMessage *aMessage) {
  gExecBase.Disable();
  mList->AddTail(*aMessage);
  gExecBase.Enable();
  mOwner->Signal(mSignalBit);
}

BMessageList::BMessageList(const char *aName) : BList(aName) {
  //
}

BMessageList::~BMessageList() {
}

BMessagePortList::BMessagePortList(const char *aName) : BListPri(aName) {
  //
}

BMessagePortList::~BMessagePortList() {
}
