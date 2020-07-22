#include <Examples/TestTask.h>
#include <Graphics/BViewPort.h>
#include <Graphics/font/BConsoleFont.h>
#include <Exec/Random.h>
#include <Inspiration/BScreen.h>

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
    win->BeginPaint();
    for (TInt i = 0; i < 10; i++) {
      win->RandomBox();
    }
    win->EndPaint();

    while (IdcmpMessage *m = win->GetMessage()) {
      dlog("MouseMove %d,%d buttons(%x)\n", m->mMouseX, m->mMouseY, m->mCode);
      m->Reply();
    }
  }
}
