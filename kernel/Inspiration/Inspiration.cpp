#include <Inspiration/Inspiration.h>
#include <Exec/ExecBase.h>
#include <Exec/BTask.h>
#include <Devices/MouseDevice.h>

class MousePointerTask : public BTask {
public:
  MousePointerTask() : BTask("MousePointer", LIST_PRI_MIN) {
    dlog("** Construct MousePointerTask\n");
    mX = mY = -1;
    mScreen = gExecBase.GetScreen();
  }

protected:
  ScreenVesa *mScreen;

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
          mScreen->MoveCursor(m->mMouseX, m->mMouseY);
          message->mReplyPort = replyPort;
          message->SendMessage(mousePort);
        }
        else {
          // shouldn't happen!
        }
      }
    }
  }

protected:
  TInt mX, mY;
};

InspirationBase::InspirationBase() {
  dlog("** Construct InspirationBase\n");
  gExecBase.AddTask(new MousePointerTask());
}

InspirationBase::~InspirationBase() {
}
