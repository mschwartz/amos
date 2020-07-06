#include <Inspiration/TestTask.h>
#include <Graphics/BViewPort.h>
#include <Graphics/font/BConsoleFont.h>
#include <Exec/Random.h>
#include <Inspiration/BScreen.h>

class ConWindow : public BConsoleWindow {
public:
  ConWindow() : BConsoleWindow("Test Console Window", 800, 40, 640, 480) {}
};

class TestTask2 : public BProcess {
public:
  TestTask2() : BProcess("Test2 Process") {}
  ~TestTask2() {}

public:
  void Run() {
    BScreen *screen = mInspirationBase.FindScreen();

    dprint("\n");
    dlog("TestTask2 Run\n");

    ConWindow *con = new ConWindow();
    screen->AddWindow(con);

    FileDescriptor *fd;
    while (ETrue) {

#if 1
      con->BeginPaint();
      con->Write("> ");
      con->EndPaint();

      Sleep(4);
      con->BeginPaint();
      con->Write("ls /fonts\n");
      con->EndPaint();

      fd = OpenDirectory("/fonts");
      if (!fd) {
        dprint("\n\n");
        dlog("*** Could not open directory /fonts\n");
        dprint("\n\n");
      }
      else {
        const DirectoryStat *s = fd->Stat();
        const char *fn = fd->Filename();
        dlog("Dirctory of %s\n", fn);

        int count = 0;
        while (ReadDirectory(fd) && count++ < 180) {
          char buf[512];
          const DirectoryStat *s = fd->Stat();
          DirectoryStat::Dump(s, fd->Filename(), buf);
          con->BeginPaint();
          con->Write(buf);
          con->EndPaint();
        }
      }
      CloseDirectory(fd);
#endif

#if 1
      con->BeginPaint();
      con->Write("\n\n> ");
      con->EndPaint();

      Sleep(4);
      con->BeginPaint();
      con->Write("cat /fonts/README.psfu\n");
      con->EndPaint();

      fd = OpenFile("/fonts/README.psfu");
      if (!fd) {
        dprint("\n\n");
        dlog("*** Could not open /fonts/README.psfu\n");
        dprint("\n\n");
      }
      else {
        char buf[512];
        while (ETrue) {
          TUint64 actual = ReadFile(fd, buf, 512);
          if (actual == 0) {
            break;
          }
          // dhexdump(buf, 32);
          for (TUint64 x = 0; x < actual; x++) {
            if (buf[x] == '\n') {
              con->BeginPaint();
              con->Write(buf[x]);
              con->EndPaint();
            }
            else {
              con->Write(buf[x]);
            }
          }
        }
        CloseFile(fd);
      }
#endif
      // Sleep(50);
    }
  }
};

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
    mClientRect.Dump();
  }

public:
  void Paint() {
    dlog("TestWidow PAINT\n");
    Clear(0x000000);
  }
  void RandomBox() {
    TRect rect;
    TRGB color(Random64(0, 255), Random64(0, 255), Random64(0, 255));
    rect.x1 = Random64(0, mClientRect.Width());
    rect.x2 = Random64(rect.x1, mClientRect.Width());
    rect.y1 = Random64(0, mClientRect.Height());
    rect.y2 = Random64(rect.y1, mClientRect.Height());
    // rect.x1 = Random64(mClientRect.x1, mClientRect.x2);
    // rect.x2 = Random64(rect.x1, mClientRect.x2);
    // rect.y1 = Random64(mClientRect.y1, mClientRect.y2);
    // rect.y2 = Random64(rect.y1, mClientRect.y2);
    mViewPort->FillRect(color, rect);
  }
};

void TestTask::Run() {
  dprint("\n");
  dlog("***************************** TEST TASK RUNNING\n");
  Sleep(1);

  // while (1)
  //   Sleep(1);

  // Display &display = mInspirationBase.GetDisplay();
  // display.Clear(0x4f4fff);

  BScreen *screen = mInspirationBase.FindScreen();

  TestWindow *win = new TestWindow();
  screen->AddWindow(win);

  dprint("Construct TestTask2\n");
  gExecBase.AddTask(new TestTask2());

  TInt count = 0;
  while (ETrue) {
    win->BeginPaint();
    for (TInt i = 0; i < 10; i++) {
      win->RandomBox();
    }
    win->EndPaint();
    while (IdcmpMessage *m = win->GetMessage()) {
      dlog("MouseMove %d,%d buttons(%x)\n", m->mMouseX, m->mMouseY, m->mCode);
      m->ReplyMessage();
    }
  }
}
