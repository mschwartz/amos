#include <Exec/ExecBase.h>
#include <Inspiration/Display.h>
#include <Inspiration/BScreen.h>
#include <Inspiration/Display/Cursor.h>
#include <Inspiration/Display/DisplayTask.h>

void Display::Init() {
  DisplayTask *task = new DisplayTask(*this);
  gExecBase.AddTask(task);
}

void Display::AddScreen(BScreen *aScreen) {
  mScreenList->AddHead(*aScreen);
  aScreen->Clear(0x4f4fff);
}

BScreen *Display::FindScreen(const char *aTitle) {
  return mScreenList->Find(aTitle);
}

BScreen *Display::TopScreen() {
  return mScreenList->First();
}
//

void Display::MoveCursor(TInt aX, TInt aY) {
  mLastX = mMouseX;
  mLastY = mMouseY;

  mMouseX = aX;
  mMouseY = aY;

  // if (mMouseX == aX && mMouseY == aY) {
  //   return;
  // }

  // BScreen *screen = TopScreen();
  // screen->AddDirtyRect(mMouseX, mMouseY,
  //   mMouseX + mCursor->Width() - 1, mMouseX + mCursor->Height() - 1);

  // dlog("Move Cursor from %d,%d to %d,%d\n", mLastX, mLastY, mMouseX, mMouseY);
  // screen->AddDirtyRect(mMouseX, mMouseY, mMouseX + mCursor->Width() - 1,
  //   mMouseX + mCursor->Height() - 1);
}

void Display::RestoreCursor() {
  mCursor->Restore(mBitmap, mLastX, mLastY);
  mCursor->AddDirtyRect(mScreenList->First(), mLastX, mLastY);
  mLastX = mMouseX;
  mLastY = mMouseY;
}

void Display::SaveCursor() {
  mCursor->Save(mBitmap, mMouseX, mMouseY);
}

void Display::RenderCursor() {
  mCursor->Render(mBitmap, mMouseX, mMouseY);
  mCursor->AddDirtyRect(mScreenList->First(), mMouseX, mMouseY);
}

TBool Display::ShowCursor() {
  TBool ret = mMouseHidden;
  mMouseHidden = EFalse;
  return ret;
}

TBool Display::HideCursor() {
  TBool ret = mMouseHidden;
  mMouseHidden = ETrue;
  return ret;
}

void Display::Clear(TUint32 aColor) {
  mBitmap->Clear(aColor);
  //
}

BWindow *Display::ActiveWindow() { return TopScreen()->ActiveWindow(); }
Display::Display() : BNode("Display") {
  dlog("Construct Display\n");
  mScreenList = new BScreenList;

  TSystemInfo info;
  gExecBase.GetSystemInfo(&info);

  TUint64 fb = (TUint64)info.mDisplayFrameBuffer;
  mBitmap = new BBitmap32(
    info.mDisplayWidth, info.mDisplayHeight, info.mDisplayPitch,
    (TAny *)fb);

  mCursor = new Cursor(); // default cursor
  mMouseX = mLastX = 20;
  mMouseY = mLastY = 20;
}

Display::~Display() {
  // should never happen!
  // (Maybe if external display is unplugged?)
  bochs;

  delete mBitmap;
  delete mScreenList;
}
