#include <Examples/ConsoleWindowTest.hpp>

class ConWindow : public BConsoleWindow {
public:
  ConWindow() : BConsoleWindow("Test Console Window", 800, 40, 640, 480) {}
};

TInt64 ConsoleWindowTestTask::Run() {

  dprint("\n");
  dlog("***************************** ConsoleWindowTestTask Running\n");

  Sleep(2);
  
  BScreen *screen = mInspirationBase.FindScreen();
  ConWindow *con = new ConWindow();
  screen->AddWindow(con);

  FileDescriptor *fd;
  for (;;) {

    con->Write("> ");

    HandleKeys(con);

    wait(4, con);
    con->Write("ls /fonts\n");
    dlog("ls /fonts\n");

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
	// dlog("%s\n", buf);
        con->Write(buf);
      }
    }
    CloseDirectory(fd);

    con->Write("\n\n> ");
    HandleKeys(con);

    wait(4, con);
    con->Write("cat /fonts/README.psfu\n");

    fd = OpenFile("/fonts/README.psfu");
    if (!fd) {
      dprint("\n\n");
      dlog("*** Could not open /fonts/README.psfu\n");
      dprint("\n\n");
    }
    else {
      char buf[512];
      for (;;) {
        HandleKeys(con);
        TUint64 actual = ReadFile(fd, buf, 512);
        if (actual == 0) {
          break;
        }
        // dhexdump(buf, 32);
        for (TUint64 x = 0; x < actual; x++) {
          if (buf[x] == '\n') {
            con->Write(buf[x]);
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
