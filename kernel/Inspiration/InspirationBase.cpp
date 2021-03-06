#include <Inspiration/InspirationBase.hpp>
#include <Exec/ExecBase.hpp>
#include <Exec/BTask.hpp>
#include <Exec/MessagePort.hpp>
#include <Devices/MouseDevice.hpp>
#include <Inspiration/WindowManager/MousePointerTask.hpp>
#include <Inspiration/WindowManager/KeyboardTask.hpp>
#include <Inspiration/BTheme.hpp>
#include <Examples/Examples.hpp>

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

/**
 * IdcmpMessageReaperTask
 * 
 * Idcmp Messages are sent from this task.  We need a task to have a reply port
 * to get the applications' replies, and to deal with the replied messages.
 */
class IdcmpMessageReaperTask : public BTask {
public:
  IdcmpMessageReaperTask() : BTask("IdcmpMessageRepaerTask") {
  }

public:
  TInt64 Run() {
    mPort = CreatePort("ReaperPort");
    for (;;) {
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
      // handle gimmezerozero
      if (aWindow->WindowFlags() & WFLAG_GIMMEZEROZERO) {
        aMessage->mMouseX = aMessage->mMouseX - aWindow->ClientLeft();
        aMessage->mMouseY = aMessage->mMouseY - aWindow->ClientTop();
      }
      else if (aWindow->mIdcmpFlags & IDCMP_DELTAMOVE) {
      	aMessage->mMouseX = aMessage->mMouseX - aMessage->mLastMouseX;
      	aMessage->mMouseY = aMessage->mMouseY - aMessage->mLastMouseY;
      }
      else {
      	aMessage->mMouseX = aMessage->mMouseX - aWindow->WindowLeft();
      	aMessage->mMouseY = aMessage->mMouseY - aWindow->WindowTop();
      }
      m->Send(aWindow->mIdcmpPort);
      return ETrue;
    }
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
  dprint("\n\n\n");
  dlog("********** InspirationBase Init\n");

  mThemeList.AddHead(*new BTheme("Default Theme"));

  mDisplay = new Display();
  dlog("  Init InspirationBase Display(%x)\n", mDisplay);

  mDesktop = new Desktop();
  AddScreen(mDesktop);
  dlog("  Constructed Desktop(%x)\n", mDesktop);

  mReaperTask = new IdcmpMessageReaperTask;
  gExecBase.AddTask(mReaperTask);

  gExecBase.AddTask(new MousePointerTask());
  gExecBase.AddTask(new KeyboardTask());

  mDisplay->Init();
  StartExamples();
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

void InspirationBase::UpdateWindow(BWindow *aWindow) {
  TBool hidden = mDisplay->HideCursor();
  mDisplay->BltBitmap(aWindow->mBitmap,
    aWindow->mWindowRect.x1,
    aWindow->mWindowRect.y1);
  mDisplay->SetCursor(!hidden);
}

TBool InspirationBase::SendIdcmpMessage(IdcmpMessage *aMessage) {
  if (aMessage) { // paranoia!
    // handle gimmezerozero and delta
    return mReaperTask->SendMessage(aMessage, ActiveWindow());
  }
  return EFalse;
}

TBool InspirationBase::ActivateWindow(TCoordinate aX, TCoordinate aY) {
  BScreen *s = mDisplay->TopScreen();
  return s->ActivateWindow(aX, aY);
}

BWindow *InspirationBase::DragWindow(TCoordinate aX, TCoordinate aY) {
  BScreen *s = mDisplay->TopScreen();
  return s->DragWindow(aX, aY);
}
