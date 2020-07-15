#include <Inspiration/InspirationBase.h>
#include <Inspiration/WindowManager/MousePointerTask.h>
#include <Exec/ExecBase.h>
#include <Devices/MouseDevice.h>
#include <Inspiration/NewWindow.h>

MousePointerTask::MousePointerTask()
    : BTask("MousePointer", TASK_PRI_URGENT), mDisplay(mInspirationBase.GetDisplay()) {
  dprint("\n\n");
  dlog("Construct MousePointerTask\n");
  mLastButtons = mCurrentButtons = 0;
  mMouseX = mMouseY = 20;
  mLastMouseX = mLastMouseY = 20;
}

void MousePointerTask::SendIdcmpMessage(TUint64 aClass, TUint64 aButtons, MouseMessage *aMessage) {
  IdcmpMessage *im = new IdcmpMessage;
  if (im) { // paranoia
    im->mClass = aClass;
    im->mCode = aButtons;
    im->mQualifier = 0;
    im->mAddress = ENull;
    im->mMouseX = aMessage->mMouseX;
    im->mMouseY = aMessage->mMouseY;
    im->mLastMouseX = mLastMouseX;
    im->mLastMouseY = mLastMouseY;
    mInspirationBase.SendIdcmpMessage(im);
  }
}

void MousePointerTask::HandleButtons(MouseMessage *aMessage) {
  mLastButtons = mCurrentButtons;
  mCurrentButtons = aMessage->mButtons;

  // LEFT BUTTON
  if (mCurrentButtons & MOUSE_LEFT_BUTTON) {
    // dlog("mCurrentButtons(%x / %x)\n", mCurrentButtons, aMessage->mButtons);
    // SELECT IS JUST DOWN
    if (!(mLastButtons & MOUSE_LEFT_BUTTON)) {
      if (mInspirationBase.ActivateWindow(mMouseX, mMouseY) == EFalse) {
        SendIdcmpMessage(IDCMP_MOUSEBUTTONS, SELECTDOWN, aMessage);
      }
    }
  }
  else if (mLastButtons & MOUSE_LEFT_BUTTON) {
    // was pressed, now not - SELECT released
    SendIdcmpMessage(IDCMP_MOUSEBUTTONS, SELECTUP, aMessage);
  }
  // else do nothing, button and last button not pressed

  // MIDDLE BUTTON
  if (mCurrentButtons & MOUSE_MIDDLE_BUTTON) {
    if (!(mLastButtons & MOUSE_MIDDLE_BUTTON)) {
      SendIdcmpMessage(IDCMP_MOUSEBUTTONS, MIDDLEDOWN, aMessage);
    }
  }
  else if (mLastButtons & MOUSE_MIDDLE_BUTTON) {
    // was pressed, now not
    SendIdcmpMessage(IDCMP_MOUSEBUTTONS, MIDDLEUP, aMessage);
  }
  // else do nothing, button and last button not pressed

  // RIGHT BUTTON
  if (mCurrentButtons & MOUSE_RIGHT_BUTTON) {
    if (!(mLastButtons & MOUSE_RIGHT_BUTTON)) {
      SendIdcmpMessage(IDCMP_MOUSEBUTTONS, MENUDOWN, aMessage);
    }
  }
  else if (mLastButtons & MOUSE_RIGHT_BUTTON) {
    // was pressed, now not
    SendIdcmpMessage(IDCMP_MOUSEBUTTONS, MENUUP, aMessage);
  }
  // else do nothing, button and last button not pressed
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

  MouseMessage *move_message = new MouseMessage(replyPort, EMouseMove);
  move_message->mReplyPort = replyPort;
  move_message->Send(mousePort);

  MouseMessage *buttons_message = new MouseMessage(replyPort, EMouseButtons);
  buttons_message->mReplyPort = replyPort;
  buttons_message->Send(mousePort);

  while (ETrue) {
    WaitPort(replyPort);
    while (MouseMessage *m = (MouseMessage *)replyPort->GetMessage()) {
      mLastMouseX = mMouseX;
      mLastMouseY = mMouseY;
      mMouseX = m->mMouseX;
      mMouseY = m->mMouseY;

      // note: mouse move messages do not provide mouse buttons state, just mouse x,y
      if (m == move_message) {
        // dlog("Move Cursor %d,%d\n", mMouseX, mMouseY);
        mDisplay->MoveCursor(mMouseX, mMouseY);

        SendIdcmpMessage(IDCMP_MOUSEMOVE, m->mButtons, m);

        HandleButtons(m);

        move_message->mReplyPort = replyPort;
        move_message->Send(mousePort);
      }
      else if (m == buttons_message) {
        HandleButtons(m);

        buttons_message->mReplyPort = replyPort;
        buttons_message->Send(mousePort);
      }
      else {
        dprint("\n\n");
        dlog("*** MouseTask: %x != %x or %x\n\n\n", m, move_message, buttons_message);
        // shouldn't happen!
      }
    }
  }
}