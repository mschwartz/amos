#include <Inspiration/InspirationBase.h>
#include <Inspiration/WindowManager/MousePointerTask.h>
#include <Exec/ExecBase.h>
#include <Devices/MouseDevice.h>

MousePointerTask::MousePointerTask()
    : BTask("MousePointer", LIST_PRI_MAX), mDisplay(mInspirationBase.GetDisplay()) {
  dprint("\n\n");
  dlog("Construct MousePointerTask\n");
}

void MousePointerTask::Run() {
  MessagePort *mousePort;

  dprint("\n");
  dlog("MousePointerTask Run\n");
  Forbid();
  while ((mousePort = gExecBase.FindMessagePort("mouse.device")) == ENull) {
    Sleep(1);
  }
  Permit();

  MessagePort *replyPort = CreateMessagePort("replyPort");
  MouseMessage *message = new MouseMessage(replyPort, EMouseMove);

  message->mReplyPort = replyPort;
  message->Send(mousePort);
  while (ETrue) {
    WaitPort(replyPort);
    while (MouseMessage *m = (MouseMessage *)replyPort->GetMessage()) {
      if (m == message) {
        // dlog("Move Cursor %d,%d\n", m->mMouseX, m->mMouseY);
        mDisplay->MoveCursor(m->mMouseX, m->mMouseY);
        message->mReplyPort = replyPort;
        message->Send(mousePort);
        IdcmpMessage im;
        im.mClass = IDCMP_MOUSEMOVE;
        im.mCode = m->mButtons;
        im.mQualifier = 0;
        im.mAddress = ENull;
        im.mMouseX = m->mMouseX;
        im.mMouseY = m->mMouseY;
        mInspirationBase.SendIdcmpMessage(&im);
      }
      else {
        dprint("\n\n");
        dlog("*** MouseTask: %x != %x\n\n\n", m, message);
        // shouldn't happen!
      }
    }
  }
}
