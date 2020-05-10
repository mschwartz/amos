#include <Inspiration/Inspiration.h>
#include <Exec/ExecBase.h>
#include <Exec/BTask.h>
#include <Devices/MouseDevice.h>
#include <Inspiration/TestTask.h>

class MousePointerTask : public BTask {
public:
  MousePointerTask() : BTask("MousePointer", LIST_PRI_MAX), mScreen(mInspirationBase.GetScreen()) {
//    dlog("** Construct MousePointerTask\n");
    mX = mY = -1;
  }

protected:
  ScreenVesa& mScreen;

public:
  void Run() {
    MessagePort *mousePort;

    while ((mousePort = gExecBase.FindMessagePort("mouse.device")) == ENull) {
      Sleep(1);
    }

    MessagePort *replyPort = CreateMessagePort("replyPort");
    MouseMessage *message = new MouseMessage(replyPort, EMouseMove);

    message->mReplyPort = replyPort;
    message->SendMessage(mousePort);
    while (1) {
      WaitPort(replyPort);
      while (MouseMessage *m = (MouseMessage *)replyPort->GetMessage()) {
        if (m == message) {
          dlog("Move Cursor %d,%d\n", m->mMouseX, m->mMouseY);
          mScreen.MoveCursor(m->mMouseX, m->mMouseY);
          message->mReplyPort = replyPort;
          message->SendMessage(mousePort);
        }
        else {
          dprint("\n\n");
          dlog("MouseTask: %x != %x\n\n\n", m, message);
          // shouldn't happen!
        }
      }
    }
  }

protected:
  TInt mX, mY;
};

// constructor
InspirationBase::InspirationBase() : mScreen(*new ScreenVesa) {
  dlog("** Construct InspirationBase\n");
}

InspirationBase::~InspirationBase() {
}

void InspirationBase::Init() {
  gExecBase.AddTask(new MousePointerTask());
  gExecBase.AddTask(new TestTask());
  gExecBase.DumpTasks();
}

void InspirationBase::UpdateWindow(BWindow *aWindow, TBool aDecorations) {
  TBool hidden = mScreen.HideCursor();
  mScreen.BltBitmap(aWindow->mBitmap, aWindow->mWindowRect.x1, aWindow->mWindowRect.y1);
  mScreen.SetCursor(!hidden);
}

void InspirationBase::AddWindow(BWindow *aWindow) {
  mWindowList.AddHead(*aWindow); 
  aWindow->PaintDecorations();
  aWindow->Paint();
}
