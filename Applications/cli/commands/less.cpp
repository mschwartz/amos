#include "commands.hh"

TInt64 CliTask::command_less(TInt ac, char **av) {
  mWindow->WriteFormatted("less argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}
