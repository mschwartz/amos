#include "commands.hpp"

TInt64 CliTask::command_help(TInt ac, char **av) {
  mWindow->WriteFormatted("Help\n");
  for (TInt i = 0; gCommands[i].mFunc != ENull; i++) {
    mWindow->WriteFormatted("%-10s %s\n", gCommands[i].mName, gCommands[i].mHelp);
  }
  return 0;
}
