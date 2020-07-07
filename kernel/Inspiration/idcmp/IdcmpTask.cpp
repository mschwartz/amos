#include <Inspiration/idcmp/IdcmpTask.h>
#include <Inspiration/idcmp/IdcmpSubscribers.h>
#include <Exec/ExecBase.h>
#include <Types/BList.h>
#include <Exec/MessagePort.h>
#include <Devices/KeyboardDevice.h>
#include <Inspiration/BWindow.h>
#include <Inspiration/InspirationBase.h>
#include <Devices/MouseDevice.h>

// TASK_PRI_URGENT so we should get scheduled when message received
IdcmpTask::IdcmpTask() : BTask("IdcmpTask", TASK_PRI_URGENT) {
}

IdcmpTask::~IdcmpTask() {
}

// Broadcast copies of aMessage to Active Window
void IdcmpTask::SendIdcmp(IdcmpMessage *aMessage) {
  BWindow *w = mInspirationBase.ActiveWindow();
  if (w->mIdcmpFlags & aMessage->mClass) {
    IdcmpMessage *m = AllocMessage();
    *m = *aMessage;
    m->mTime = gExecBase.SystemTicks();
    m->SendMessage(w->mIdcmpPort);
  }
}

// Instead of constant new/delete of messages, we keep replied to messages on a free list
// and try to return one of those.  If no messages on the free list, we allocate a message.
IdcmpMessage *IdcmpTask::AllocMessage() {
  IdcmpMessage *m = (IdcmpMessage *)mFreeMessages.RemHead();
  if (m) {
    return m;
  }
  return new IdcmpMessage;
}

void IdcmpTask::HandleSubscribe() {
  IdcmpSubscriber *s;
  while (IdcmpSubscribeMessage *m = (IdcmpSubscribeMessage *)mCommandPort->GetMessage()) {
    switch (m->mCommand) {
      case EIdcmpSubscribe:
        s = new IdcmpSubscriber;
        s->mPort = m->mIdcmpPort;
        s->mWindow = m->mWindow;
        s->mIdcmpFlags = m->mIdcmpFlags;
        mSubscribers->AddTail(*s);
        break;

      case EIdcmpUnsubscribe:
        s = mSubscribers->Find(m);
        if (s) {
          s->Remove();
          delete s;
        }
        break;

      case EIdcmpUpdateFlags:
        s = mSubscribers->Find(m);
        if (s) {
          s->mIdcmpFlags = m->mIdcmpFlags;
        }
        break;

      default:
        // invalid, shouldn't happen!
        break;
    }

    m->ReplyMessage();
  }
}

void IdcmpTask::HandleKeyboard() {
  while (KeyboardMessage *m = (KeyboardMessage *)mKeyboardReplyPort->GetMessage()) {
    IdcmpMessage im;
    im.mClass = IDCMP_RAWKEY;
    im.mCode = m->mResult;
    im.mQualifier = 0;
    im.mAddress = ENull;
    SendIdcmp(&im);
    m->ReplyMessage();
  }
}

void IdcmpTask::HandleMouse() {
  while (MouseMessage *m = (MouseMessage *)mMouseReplyPort->GetMessage()) {
    dlog("       MOUSE %d,%dn", m->mMouseX, m->mMouseY);
    IdcmpMessage im;
    // handle IDCMP_MOUSEMOVE
    im.mClass = IDCMP_MOUSEMOVE;
    im.mCode = m->mButtons;
    im.mQualifier = 0;
    im.mAddress = ENull;
    im.mMouseX = m->mMouseX;
    im.mMouseY = m->mMouseY;
    SendIdcmp(&im);

    // handle IDCMP_MOUSEBUTTONS
    im.mClass = IDCMP_MOUSEBUTTONS;
    SendIdcmp(&im);

    m->ReplyMessage();
    mMouseMessage->mReplyPort = mMouseReplyPort;
    mMouseMessage->SendMessage(mMousePort);
  }
}

void IdcmpTask::HandleReply() {
  while (IdcmpMessage *m = (IdcmpMessage *)mReplyPort->GetMessage()) {
    mFreeMessages.AddTail((BNode &)*m);
  }
}

void IdcmpTask::Run() {
  mSubscribers = new IdcmpSubscribers;

  mCommandPort = CreateMessagePort();    // subscribe/unsubscribe/alter messages
  mReplyPort = CreateMessagePort();      // replies to the IDCMP messages we send
  mMouseReplyPort = CreateMessagePort(); // events from mouse.device and keyboard.device (replyPort for both)
  mKeyboardReplyPort = CreateMessagePort();

  Forbid();
  while ((mMousePort = gExecBase.FindMessagePort("mouse.device")) == ENull) {
    Sleep(1);
  }
  while ((mKeyboardPort = gExecBase.FindMessagePort("keyboard.device")) == ENull) {
    Sleep(1);
  }
  Permit();

  // while (1) {
  //   Sleep(1);
  // }
  mMouseMessage = new MouseMessage(mMouseReplyPort, EMouseMove);
  mMouseMessage->mReplyPort = mMouseReplyPort;
  mMouseMessage->SendMessage(mMousePort);

  mKeyboardMessage = new KeyboardMessage(mKeyboardReplyPort, EKeyRead);
  mKeyboardMessage->mReplyPort = mKeyboardReplyPort;
  mKeyboardMessage->SendMessage(mKeyboardPort);

  while (ETrue) {
    WaitPorts(0, mCommandPort, mReplyPort, mMouseReplyPort, mKeyboardReplyPort, ENull);
    dlog("-=-=-=-= Woke!\n");
    // HandleSubscribe();
    // HandleMouse();
    // HandleKeyboard();
    // HandleReply();
  }
};
