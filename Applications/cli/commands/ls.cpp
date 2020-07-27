#include "commands.hh"

TInt64 CliTask::command_ls(TInt ac, char **av) {
  char *which = ac > 1 ? av[1] : mCurrentDirectory;

  FileDescriptor *fd;
  fd = OpenDirectory(which);
  if (!fd) {
    return Error("Could not open directory %s", which);
  }

  const DirectoryStat *s = fd->Stat();
  const char *fn = fd->Filename();

  while (ReadDirectory(fd)) {
    char buf[512];
    const DirectoryStat *s = fd->Stat();
    DirectoryStat::Dump(s, fd->Filename(), buf);
    mWindow->BeginPaint();
    mWindow->WriteFormatted("%s", buf);
    mWindow->EndPaint();
  }
  CloseDirectory(fd);
  return 0;
}
