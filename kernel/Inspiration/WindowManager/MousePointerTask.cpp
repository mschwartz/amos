#include <Inspiration/InspirationBase.hpp>
#include <Inspiration/WindowManager/MousePointerTask.hpp>
#include <Exec/ExecBase.hpp>
#include <Devices/MouseDevice.hpp>
#include <Inspiration/NewWindow.hpp>

MousePointerTask::MousePointerTask()
    : BTask("MousePointer", TASK_PRI_URGENT), mDisplay(mInspirationBase.GetDisplay()) {
  dprint("\n\n");
  dlog("Construct MousePointerTask\n");
  mLastButtons = mCurrentButtons = 0;
  mMouseX = mMouseY = 20;
  mLastMouseX = mLastMouseY = 20;
}

void MousePointerTask::SendIdcmpMessage(TUint64 aClass, TUint64 aButtons, MouseMessage *aMessage) {
  IdcmpMessage im;
  im.mClass = aClass;
  im.mCode = aButtons;
  im.mQualifier = 0;
  im.mAddress = ENull;
  im.mMouseX = aMessage->mMouseX;
  im.mMouseY = aMessage->mMouseY;
  im.mLastMouseX = mLastMouseX;
  im.mLastMouseY = mLastMouseY;
  mInspirationBase.SendIdcmpMessage(&im);
}

void MousePointerTask::HandleButtons(MouseMessage *aMessage) {
  mLastButtons = mCurrentButtons;
  mCurrentButtons = aMessage->mButtons;

  // LEFT BUTTON
  if (mCurrentButtons & MOUSE_LEFT_BUTTON) {
    if (!(mLastButtons & MOUSE_LEFT_BUTTON)) {
      // SELECT IS JUST DOWN
      if (mInspirationBase.ActivateWindow(mMouseX, mMouseY) == EFalse) {
        // click didn't activate a window, send idcmp message to whatever window is active
        SendIdcmpMessage(IDCMP_MOUSEBUTTONS, SELECTDOWN, aMessage);
      }
      else {
        dlog("Activated window\n");
      }
      if ((mDraggable.mWindow = mInspirationBase.DragWindow(mMouseX, mMouseY)) != ENull) {
        mDraggable.mDx = mMouseX - mDraggable.mWindow->WindowLeft();
        mDraggable.mDy = mMouseY - mDraggable.mWindow->WindowTop();
        dlog("Start Drag (%s) dx(%d) dy(%d)\n", mDraggable.mWindow->Title(), mDraggable.mDx, mDraggable.mDy);
      }
    }
    else {
      // button being held down
      if (mDraggable.mWindow) {
        // dlog("dragging %d,%d\n", mMouseX - mDraggable.mDx, mMouseY - mDraggable.mDy);
        mDraggable.mWindow->MoveTo(mMouseX - mDraggable.mDx, mMouseY - mDraggable.mDy);
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

TInt64 MousePointerTask::Run() {
  dprint("\n");
  dlog("MousePointerTask Run\n");

  MessagePort *mousePort = WaitForPort("mouse.device");
  MessagePort *replyPort = CreatePort("replyPort");

  MouseMessage *move_message = new MouseMessage(replyPort, EMouseMove);
  move_message->mReplyPort = replyPort;
  move_message->Send(mousePort);

  MouseMessage *buttons_message = new MouseMessage(replyPort, EMouseButtons);
  buttons_message->mReplyPort = replyPort;
  buttons_message->Send(mousePort);

  for (;;) {
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

        HandleButtons(m);

        if (mDraggable.mWindow) {
          //
          bochs;
        }
        else {
          SendIdcmpMessage(IDCMP_MOUSEMOVE, m->mButtons, m);
        }

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
