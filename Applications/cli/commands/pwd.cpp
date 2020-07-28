#include "commands.hpp"


TInt64 CliTask::command_pwd(TInt ac, char **av) {
  if (ac > 1) {
    return Error("%s too many arguments", av[0]);
  }
  mWindow->WriteFormatted("%s\n", mCurrentDirectory);
  return 0;
}
