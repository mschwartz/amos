#include <Examples/TestIdcmp.hpp>
#include <Graphics/BViewPort.hpp>
#include <Graphics/font/BConsoleFont.hpp>
#include <Exec/Random.hpp>
#include <Inspiration/BScreen.hpp>
#include <posix/sprintf.h>

class IdcmpWindow : public BWindow {
public:
  IdcmpWindow() : BWindow(TNewWindow{
                    .mLeft = 10,
                    .mTop = 500,
                    .mWidth = 800,
                    .mHeight = 500,
                    .mTitle = "IDCMP Test Window",
                    .mIdcmpFlags = IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS | IDCMP_VANILLAKEY,
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

TInt64 TestIdcmpTask::Run() {
  dlog("TestIdcmpTask::Run\n");
  char buf[512];

  Sleep(3);
  IdcmpWindow *win = new IdcmpWindow();
  win->Activate();

  for (;;) {
    WaitPort(win->mIdcmpPort);

    while (IdcmpMessage *m = win->GetMessage()) {
      switch (m->mClass) {

        case IDCMP_MOUSEMOVE:
          sprintf(buf, "MouseMove %d,%d buttons(%x)", m->mMouseX, m->mMouseY, m->mCode);
          win->BeginPaint();
          win->Print(0, 0, buf);
          win->EndPaint();
          break;

        case IDCMP_MOUSEBUTTONS:
          switch (m->mCode) {
            case SELECTDOWN:
              sprintf(buf, "MouseButtons %-10s %d,%d buttons(%x)", "SELECTDOWN", m->mMouseX, m->mMouseY, m->mCode);
              break;
            case SELECTUP:
              sprintf(buf, "MouseButtons %-10s %d,%d buttons(%x)", "SELECTUP", m->mMouseX, m->mMouseY, m->mCode);
              break;
            case MIDDLEDOWN:
              sprintf(buf, "MouseButtons %-10s %d,%d buttons(%x)", "MIDDLEDOWN", m->mMouseX, m->mMouseY, m->mCode);
              break;
            case MIDDLEUP:
              sprintf(buf, "MouseButtons %-10s %d,%d buttons(%x)", "MIDDLEUP", m->mMouseX, m->mMouseY, m->mCode);
              break;
            case MENUDOWN:
              sprintf(buf, "MouseButtons %-10s %d,%d buttons(%x)", "MENUDOWN", m->mMouseX, m->mMouseY, m->mCode);
              break;
            case MENUUP:
              sprintf(buf, "MouseButtons %-10s %d,%d buttons(%x)", "MENUUP", m->mMouseX, m->mMouseY, m->mCode);
              break;
          }
          // dlog("%s\n", buf);
          win->BeginPaint();
          win->Print(0, 20, buf);
          win->EndPaint();
          break;

        case IDCMP_VANILLAKEY:
          sprintf(buf, "VANILLAKEY (%02x) %c", m->mCode & 0x7f, m->mCode & 0x7f);
          win->BeginPaint();
          win->Print(0, 40, buf);
          win->EndPaint();
      }
      m->Reply();
    }
  }
}
