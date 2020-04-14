#include <BMessagePort.h>

BMessage::BMessage(BMessagePort& aReplyPort) : BNodePri("Message", 0), mReplyPort(aReplyPort) {
  //
}

BMessage::~BMessage() {
  //
}

void BMessage::Reply(BMessagePort *aReplyPort) {
  if (aReplyPort == ENull) {
    aReplyPort = &mReplyPort;
  }
  aReplyPort->SendMessage(this);
}

void BMessage::Send(BMessagePort *aMessagePort) {
  aMessagePort->SendMessage(this);
}

BMessagePort::BMessagePort(TUint64 aSignal, const char *aName) :  BNode(aName) { 
  mSignal = aSignal;
}

BMessagePort::~BMessagePort() {
};



