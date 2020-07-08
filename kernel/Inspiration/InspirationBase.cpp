#include <Inspiration/InspirationBase.h>
#include <Exec/ExecBase.h>
#include <Exec/BTask.h>
#include <Exec/MessagePort.h>
#include <Devices/MouseDevice.h>
#include <Inspiration/WindowManager/MousePointerTask.h>
#include <Inspiration/WindowManager/KeyboardTask.h>
#include <Examples/Examples.h>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class IdcmpMessageReaperTask : public BTask {
public:
  IdcmpMessageReaperTask() : BTask("IdcmpMessageRepaerTask") {
  }

public:
  void Run() {
    mPort = CreateMessagePort("ReaperPort");
    while (ETrue) {
      WaitPort(mPort);
      while (IdcmpMessage *m = (IdcmpMessage *)mPort->GetMessage()) {
	// dlog(" Reap(%x)\n", m);
        FreeMessage(m);
      }
    }
  }

protected:
  MessagePort *mPort;

protected:
  void FreeMessage(IdcmpMessage *m) {
    delete m;
  }

  IdcmpMessage *AllocMessage(IdcmpMessage *aMessage = ENull) {
    IdcmpMessage *m = new IdcmpMessage;
    if (aMessage) {
      *m = *aMessage;
    }
    m->mReplyPort = mPort;
    return m;
  }

public:
  TBool SendMessage(IdcmpMessage *aMessage, BWindow *aWindow) {

    if (aWindow->mIdcmpFlags & aMessage->mClass) {
      IdcmpMessage *m = AllocMessage(aMessage);
      m->mWindow = aWindow;
      m->mTime = gExecBase.SystemTicks();
      m->Send(aWindow->mIdcmpPort);
      return ETrue;
    }
    // else {
    //   dlog("not sent\n");
    // }
    return EFalse;
  }
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

// constructor
InspirationBase::InspirationBase() {
}

InspirationBase::~InspirationBase() {
}

void InspirationBase::Init() {
  dprint("\n");
  dlog("InspirationBase Init\n");
  mDisplay = new Display();
  dlog("  Init InspirationBase Display(%x)\n", mDisplay);

  mDesktop = new Desktop();
  AddScreen(mDesktop);
  dlog("  Constructed Desktop(%x)\n", mDesktop);

  mReaperTask = new IdcmpMessageReaperTask;
  gExecBase.AddTask(mReaperTask);

  gExecBase.AddTask(new MousePointerTask());
  gExecBase.AddTask(new KeyboardTask());

  StartExamples();
  mDisplay->Init();
}

void InspirationBase::AddScreen(BScreen *aScreen) {
  mDisplay->AddScreen(aScreen);
}

BScreen *InspirationBase::FindScreen(const char *aTitle) {
  if (aTitle == ENull) {
    return mDesktop;
  }

  return mDisplay->FindScreen(aTitle);
}

void InspirationBase::UpdateWindow(BWindow *aWindow, TBool aDecorations) {
  TBool hidden = mDisplay->HideCursor();
  mDisplay->BltBitmap(aWindow->mBitmap,
    aWindow->mWindowRect.x1,
    aWindow->mWindowRect.y1);
  mDisplay->SetCursor(!hidden);
}

TBool InspirationBase::SendIdcmpMessage(IdcmpMessage *aMessage) {
  if (aMessage) { // paranoia!
    return mReaperTask->SendMessage(aMessage, ActiveWindow());
  }
  return EFalse;
}
