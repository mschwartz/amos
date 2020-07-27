#include "commands.hpp"


TInt64 CliTask::command_lspci(TInt ac, char **av) {
  mWindow->WriteFormatted("lspci argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}
