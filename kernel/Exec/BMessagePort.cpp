#include <BMessagePort.h>

BMessage::BMessage(BMessagePort& aReplyPort) : BNodePri(0), mReplyPort(aReplyPort) {
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

BMessagePort::BMessagePort(TUint64 aSignal) :  BBase() { 
  mSignal = aSignal;
}

BMessagePort::~BMessagePort() {
};



