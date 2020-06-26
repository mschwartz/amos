#include <Exec/ExecBase.h>
#include <Inspiration/Display.h>
#include <Inspiration/Mouse.h>
#include <Inspiration/BScreen.h>
#include <Exec/BTask.h>

/**
 * DisplayTask
 * 
 * Initialization figures out a rough estimate of how many milliseconds between vblanks.
 *
 * Loops - waiting that many milliseconds, then busy waiting on the vblank bit in 0x3da
 *   and then updates the dirty rectangles for the current screen.
 */
class DisplayTask : public BTask {
public:
  DisplayTask(Display &aDisplay)
      : BTask("DisplayTask", TASK_PRI_MAX), mDisplay(aDisplay) {
    //
  }

public:
  void Run() {
    dlog("DisplayTask Alive %x\n", inb(0x3da));
    Sleep(1);
    // dlog("  slept\n");
    // wait for not in vblank
    while ((inb(0x3da) & 0x08) != 0)
      ;
    TUint64 start = gExecBase.SystemTicks();
    // wait for in vblank
    while ((inb(0x3da) & 0x08) == 0)
      ;

    TUint64 end = gExecBase.SystemTicks();
    TUint64 vbl_time = end - start;
    dlog("  start(%d) end(%d) vbl_time(%d)\n", start, end, vbl_time);
    if (vbl_time == 0) {
      vbl_time = 16;
    }

    // wait for vbl in a loop and update screen via DirtyRects
    while (ETrue) {
      MilliSleep(vbl_time);
      // wait for in vblank
      while ((inb(0x3da) & 0x08) == 0)
        ;
      TBool shown = mDisplay.HideCursor();
      // update dirty rects
      BScreen *screen = mDisplay.mScreenList->First();
      screen->UpdateDirtyRects();
      mDisplay.SetCursor(shown);
    }
  }

protected:
  Display &mDisplay;
};

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

//

void Display::MoveCursor(TInt aX, TInt aY) {
  mMouse->MoveTo(aX, aY);
}

TBool Display::ShowCursor() {
  return mMouse->Show();
}

TBool Display::HideCursor() {
  return mMouse->Hide();
}

void Display::Clear(TUint32 aColor) {
  mBitmap->Clear(aColor);
  //
}

#if 0
void Display::ClearEOL(TUint8 aCharacter) {
  //
}

void Display::Down() {
  //
}

void Display::ScrollUp() {
  //
}

void Display::NewLine() {
  //
}

void Display::WriteChar(char c) {
  //
}


void Display::WriteString(TInt aX, TInt aY, const char *s) {
  TBool hidden = HideCursor();
  mBitmap->DrawText(aX, aY, s);
  SetCursor(!hidden);
}

void Display::WriteString(const char *s) {
  if (!mMouseHidden) {
    HideCursor();
    mBitmap->DrawText(mX, mY, s);
    ShowCursor();
  }
  else {
    mBitmap->DrawText(mX, mY, s);
  }
}

#endif

Display::Display() : BNode("Display") {
  dlog("Construct Display\n");
  mScreenList = new BScreenList;
  TSystemInfo info;
  gExecBase.GetSystemInfo(&info);

  TUint64 fb = (TUint64)info.mDisplayFrameBuffer;
  mBitmap = new BBitmap32(info.mDisplayWidth, info.mDisplayHeight, info.mDisplayPitch, (TAny *)fb);
  // mBitmap->Dump();

  mMouse = new Mouse(this);
}

Display::~Display() {
  // should never happen!
  // (Maybe if external display is unplugged?)
  bochs;

  delete mMouse;
  delete mBitmap;
  delete mScreenList;
}
