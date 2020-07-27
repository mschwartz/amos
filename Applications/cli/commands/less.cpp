#include "commands.hpp"

TInt64 CliTask::command_less(TInt ac, char **av) {
  if (ac != 2) {
    return Error("%s requires 1 argument", av[0]);
  }

  FileDescriptor *fd;
  fd = OpenFile(av[1]);
  if (!fd) {
    return Error("Could not open %s", av[1]);
  }
  else {
    char buf[512];
    TInt count = 0;
    for (;;) {
      TUint64 actual = ReadFile(fd, buf, 512);
      if (actual == 0) {
        break;
      }
      // TODO: count lines, use mWindow console height (rows)
      for (TUint64 x = 0; x < actual; x++) {
        if (buf[x] == '\n') {
          mWindow->BeginPaint();
          mWindow->Write(buf[x]);
          mWindow->EndPaint();
	  count++;
          if (count >= mWindow->Rows()) {
	    count = 0;
          }
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
