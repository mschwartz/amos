#include "commands.hpp"


TInt64 CliTask::command_lsahci(TInt ac, char **av) {
  if (ac > 2) {
    Error("cat expects only one argument");
  }
  mWindow->WriteFormatted("lsahci argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}
