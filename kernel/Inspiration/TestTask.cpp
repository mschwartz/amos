#include <Inspiration/TestTask.h>
#include <Graphics/BViewPort.h>
#include <Graphics/font/BConsoleFont.h>
#include <Exec/Random.h>

TestTask::TestTask() : BProcess("Test Process") {
  dprint("Construct TestTask\n");
}

TestTask::~TestTask() {
  //
}

class TestWindow : public BWindow {
public:
  TestWindow() : BWindow("Test Window", 10, 10, 640, 480) {}

public:
  void Paint() {
    dlog("PAINT\n");
    Clear(0x000000);
  }
  void RandomBox() {
    TRect rect;
    TRGB color(Random64(0, 255), Random64(0, 255), Random64(0, 255));
    rect.x1 = Random64(mClientRect.x1, mClientRect.x2);
    rect.x2 = Random64(rect.x1, mClientRect.x2);
    rect.y1 = Random64(mClientRect.y1, mClientRect.y2);
    rect.y2 = Random64(rect.y1, mClientRect.y2);
    mViewPort->FillRect(color, rect);
  }
};

void TestTask::Run() {
  dlog("***************************** TEST TASK RUNNING\n");
  Sleep(1);

  FileDescriptor *fd = OpenDirectory("/fonts");
  if (!fd) {
    dlog("Could not open directory /fonts\n");
  }
  else {
    const DirectoryStat *s = fd->Stat();
    const char *fn = fd->Filename();
    dlog("Dirctory of %s\n", fn);

    int count = 0;
    while (ReadDirectory(fd) && count++ < 180) {
      const DirectoryStat *s = fd->Stat();
      DirectoryStat::Dump(s, fd->Filename());
    }
  }
  CloseDirectory(fd);

  fd = OpenFile("/fonts/README.psfu");
  if (!fd) {
    dlog("Could not open /fonts/README.psfu\n");
  }
  else {
    dlog("Opened REAFME\n");
    fd->Dump();
  }
  ScreenVesa &screen = mInspirationBase.GetScreen();
  screen.Clear(0x4f4fff);

  TestWindow *win = new TestWindow();
  mInspirationBase.AddWindow(win);

  dlog("LOOP %x\n", GetFlags());
  TInt count = 0;
  while (1) {
    win->BeginPaint();
    for (TInt i = 0; i < 10; i++) {
      win->RandomBox();
    }
    win->EndPaint();
  }
#if 0
#ifdef KGFX
  ScreenVesa &screen = *gExecBase.GetScreen();
  BBitmap32 &bm = *screen.GetBitmap();
  //    bm.Dump();
  BViewPort32 *vp = new BViewPort32("test vp", &bm);
  TRect rect, screenRect;
  bm.GetRect(screenRect);

  TRGB fg(255, 255, 255), bg(0, 0, 0);
  BConsoleFont32 font(&bm);

  bm.SetFont(&font);
  font.SetColors(fg, bg);
  vp->SetFont(&font);
  vp->SetColors(fg, bg);
  TRect vrect(50, 200, 500, 300);
  vp->SetRect(vrect);

  RtcDevice *rd = (RtcDevice *)gExecBase.FindDevice("rtc.device");
  //    if (!rd) {
  //      dprint("Can't find rct.device\n");
  //      halt();
  //    }
  bm.Clear(0x0000ff);
#if 0
      TRGB color;

      while (ETrue) {
        dlog("START!\n");
        for (TInt n=0; n<100000; n++) {
          if ((n % 100) == 0) {
            dlog("n = %d\n", n);
          }
          rect.x1 = Random(screenRect.x1, screenRect.x2);
          rect.x2 = Random(rect.x1, screenRect.x2);
          rect.y1 = Random(screenRect.y1, screenRect.y2);
          rect.y2 = Random(rect.y1, screenRect.y2);
          TRGB color(Random(0, 255), Random(0, 255), Random(0, 255));
//          dlog("Fill %d,%d,%d,%d %x\n", rect.x1, rect.y1, rect.x2, rect.y2, color.rgb888());
          bm.FillRect(color, rect);
//          bm.DrawRect(color, rect);
        }
        dlog("END!\n");
        Sleep(1);
      }
#else
  bm.FillRect(0xffffff, 300, 300, 500, 500);
  //    TInt count = 0;
  while (true) {
    //      dlog("test task loop %d\n", ++count);;
    char buf[128];
    sprintf(buf, "%02d/%02d/%02d %02d:%02d:%02d.%d", rd->mMonth, rd->mDay, rd->mYear, rd->mHours, rd->mMinutes, rd->mSeconds, rd->mFract);
    //      dlog("buf: %s\n", buf);
    screen.HideCursor();
    vp->DrawText(0, 0, buf);
    screen.ShowCursor();
    //      font.Write(vp, 100, 100, buf);
    Sleep(1);
  }
#endif
#else
  RtcDevice *rd = ENull;
  while (rd == ENull) {
    rd = (RtcDevice *)gExecBase.FindDevice("rtc.device");
    if (rd) {
      break;
    }
    Sleep(1);
  }
  ScreenVGA &screen = *gExecBase.GetScreen();
  screen.MoveTo(20, 20);
  dprint("Test Task\n");
  while (true) {
    char buf[128];
    sprintf(buf, "%02d/%02d/%02d %02d:%02d:%02d.%d", rd->mMonth, rd->mDay, rd->mYear, rd->mHours, rd->mMinutes, rd->mSeconds, rd->mFract);
    screen.MoveTo(10, 10);
    dprint(buf);
    Sleep(1);
  }

#endif

  //      TInt64 time = 0;
  //      while (1) {
  //        Sleep(1);
  //        dlog("TestTask: Time %d\n", ++time);
  //      }
#endif
}
