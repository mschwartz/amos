#include "commands.hpp"

TInt64 CliTask::command_ps(TInt ac, char **av) {
  mWindow->WriteFormatted("ps argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}
