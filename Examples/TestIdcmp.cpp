#include <Examples/TestIdcmp.h>
#include <Graphics/BViewPort.h>
#include <Graphics/font/BConsoleFont.h>
#include <Exec/Random.h>
#include <Inspiration/BScreen.h>

class IdcmpWindow : public BWindow {
public:
  IdcmpWindow() : BWindow(TNewWindow{
                    .mLeft = 10,
                    .mTop = 500,
                    .mWidth = 800,
                    .mHeight = 500,
                    .mTitle = "IDCMP Test Window",
                    .mIdcmpFlags = IDCMP_MOUSEMOVE | IDCMP_VANILLAKEY,
                  }) {
  }

  void Print(TInt aX, TInt aY, const char *aString) {
    // dprint("DrawText(%d,%d) (%s)\n", aX, aY, aString);
    mViewPort->DrawText(aX, aY, aString);
  }
};

TestIdcmpTask::TestIdcmpTask() : BTask("Idcmp Test") {
}

TestIdcmpTask::~TestIdcmpTask() {
}

void TestIdcmpTask::Run() {
  char buf[512];

  Sleep(3);
  IdcmpWindow *win = new IdcmpWindow();
  win->Activate();

  while (ETrue) {
    WaitPort(win->mIdcmpPort);
    win->BeginPaint();
    while (IdcmpMessage *m = win->GetMessage()) {
      if (m->mClass & IDCMP_MOUSEMOVE) {
        sprintf(buf, "MouseMove %d,%d buttons(%x)", m->mMouseX, m->mMouseY, m->mCode);
        win->Print(0, 0, buf);
        // dprint("%\n", buf);
      }
      else if (m->mClass & IDCMP_VANILLAKEY) {
        sprintf(buf, "VANILLAKEY (%02x) %c", m->mCode & 0x7f, m->mCode & 0x7f);
        win->Print(0, 20, buf);
        // dprint("%\n", buf);
      }
      m->Reply();
    }
    win->EndPaint();
  }
}
