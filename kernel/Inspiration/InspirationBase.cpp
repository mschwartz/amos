#include <Inspiration/InspirationBase.h>
#include <Exec/ExecBase.h>
#include <Exec/BTask.h>
#include <Devices/MouseDevice.h>
#include <Inspiration/WindowManager/MousePointerTask.h>
#include <Inspiration/WindowManager/KeyboardTask.h>
#include <Inspiration/TestTask.h>

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

  gExecBase.AddTask(new MousePointerTask());
  // gExecBase.AddTask(new IdcmpTask());
  gExecBase.AddTask(new TestTask());

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
  BWindow *w = ActiveWindow();
  if (w->mIdcmpFlags & aMessage->mClass) {
    aMessage->Send(w->mIdcmpPort);
    return ETrue;
  }
  return EFalse;
}
