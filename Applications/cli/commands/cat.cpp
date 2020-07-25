#include "commands.hh"

TInt64 CliTask::command_cat(TInt ac, char **av) {
  mWindow->WriteFormatted("cat argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}
