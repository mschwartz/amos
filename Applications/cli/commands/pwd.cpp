#include "commands.hh"


TInt64 CliTask::command_pwd(TInt ac, char **av) {
  mWindow->WriteFormatted("pwd argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}
