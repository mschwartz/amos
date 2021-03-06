#include <Examples/TestTask.hpp>
#include <Graphics/BViewPort.hpp>
#include <Graphics/font/BConsoleFont.hpp>
#include <Exec/Random.hpp>
#include <Inspiration/BScreen.hpp>

TestTask::TestTask() : BProcess("Test Process") {
}

TestTask::~TestTask() {
  //
}

class TestWindow : public BWindow {
public:
  // TestWindow() : BWindow("Test Window", 10, 10, 640, 480) {}
  TestWindow()
      : BWindow(TNewWindow{
          .mLeft = 10,
          .mTop = 10,
          .mWidth = 640,
          .mHeight = 400,
          .mTitle = "Test Window",
          .mIdcmpFlags = IDCMP_MOUSEMOVE,
        }) {
  }

public:
  void RandomBox() {
    TRect rect;
    TRGB color(Random64(0, 255), Random64(0, 255), Random64(0, 255));
    rect.x1 = Random64(0, mClientRect.Width());
    rect.x2 = Random64(rect.x1, mClientRect.Width());
    rect.y1 = Random64(0, mClientRect.Height());
    rect.y2 = Random64(rect.y1, mClientRect.Height());
    mViewPort->FillRect(color, rect);
  }
};

TInt64 TestTask::Run() {
  dprint("\n");
  dlog("***************************** TEST TASK RUNNING\n");

  BScreen *screen = mInspirationBase.FindScreen();

  TestWindow *win = new TestWindow();
  screen->AddWindow(win);

  for (;;) {
    win->RandomBox();

    while (IdcmpMessage *m = win->GetMessage()) {
      dlog("MouseMove %d,%d buttons(%x)\n", m->mMouseX, m->mMouseY, m->mCode);
      m->Reply();
    }
  }
}
