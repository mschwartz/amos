#include "commands.hpp"

TInt64 CliTask::command_cd(TInt ac, char **av) {
  if (ac > 2) {
    return Error("%s too many arguments", av[0]);
  }

  if (ac == 0) {
    CopyString(mCurrentDirectory, "/");
  }
  else {
    // TODO: test if av[1] is directory
    CopyString(mCurrentDirectory, av[1]);
  }

  return 0;
}
