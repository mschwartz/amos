#include <Examples/ConsoleWindowTest.h>
// #include <Inspiration/BConsoleWindow.h>

class ConWindow : public BConsoleWindow {
public:
  ConWindow() : BConsoleWindow("Test Console Window", 800, 40, 640, 480) {}
};

void ConsoleWindowTestTask::Run() {

  dprint("\n");
  dlog("***************************** ConsoleWindowTestTask Running\n");

  Sleep(2);
  
  BScreen *screen = mInspirationBase.FindScreen();
  ConWindow *con = new ConWindow();
  screen->AddWindow(con);

  FileDescriptor *fd;
  while (ETrue) {

    con->BeginPaint();
    con->Write("> ");
    con->EndPaint();

    HandleKeys(con);

    wait(4, con);
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

      int count = 0;
      while (ReadDirectory(fd) && count++ < 180) {
        HandleKeys(con);
        char buf[512];
        const DirectoryStat *s = fd->Stat();
        DirectoryStat::Dump(s, fd->Filename(), buf);
        con->BeginPaint();
        con->Write(buf);
        con->EndPaint();
      }
    }
    CloseDirectory(fd);

    con->BeginPaint();
    con->Write("\n\n> ");
    con->EndPaint();
    HandleKeys(con);

    wait(4, con);
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
        HandleKeys(con);
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
  }
}
