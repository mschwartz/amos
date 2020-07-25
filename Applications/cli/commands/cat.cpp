#include "commands.hh"

TInt64 CliTask::command_cat(TInt ac, char **av) {
  if (ac != 2) {
    return Error("cat requires 1 argument");
  }
  FileDescriptor *fd;
  fd = OpenFile(av[1]);
  if (!fd) {
    return Error("Could not open %s", av[1]);
  }
  else {
    char buf[512];
    for (;;) {
      TUint64 actual = ReadFile(fd, buf, 512);
      if (actual == 0) {
        break;
      }
      // dhexdump(buf, 32);
      for (TUint64 x = 0; x < actual; x++) {
        if (buf[x] == '\n') {
          mWindow->BeginPaint();
          mWindow->Write(buf[x]);
          mWindow->EndPaint();
        }
        else {
          mWindow->Write(buf[x]);
        }
      }
    }
    CloseFile(fd);
  }
  return 0;
}
