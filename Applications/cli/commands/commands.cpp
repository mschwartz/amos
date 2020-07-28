#include "commands.hpp"

struct cli_commands  gCommands[] = {
  "help", &CliTask::command_help, "print help (for command)",
  "cd", &CliTask::command_cd, "change current directory",
  "pwd", &CliTask::command_pwd, "print cwd",
  "lsahci", &CliTask::command_lsahci, "list ahci table",
  "lspci", &CliTask::command_lspci, "list pci devices",
  "sysinfo", &CliTask::command_sysinfo, "print system information",
  "cpu", &CliTask::command_cpu, "print CPU information",
  "date", &CliTask::command_date, "print date and time",
  "uptime", &CliTask::command_uptime, "ptime - print uptime",
  "df", &CliTask::command_df, "print disk infomration about devices",
  "ps", &CliTask::command_ps, "list Tasks in system",
  "theme", &CliTask::command_theme, "print desktop theme information",
  // files
  "ls", &CliTask::command_ls, "print directory info of files (or cwd)",
  "touch", &CliTask::command_touch, "touch files (update mtime or create if not exists)",
  "cat", &CliTask::command_cat, "type file to screen",
  "less", &CliTask::command_less, "type file to screen with pager",
  ENull, ENull
};

TInt64 CliTask::ExecuteCommand(char *aCommand) {
  const int MAXARGS = 64;
  TInt ac = 0;
  char *av[MAXARGS];
  char *p1 = aCommand;
  while (*p1 != '\0') {
    char *p2 = p1;
    while (*p2 != ' ') {
      if (*p2 == '\0') {
        break;
      }
      p2++;
    }
    av[ac++] = p1;
    if (*p2 == ' ') {
      *p2++ = '\0';
    }
    p1 = p2;
  }

  for (TInt i = 0; gCommands[i].mFunc != ENull; i++) {
    if (CompareStrings(av[0], gCommands[i].mName) == 0) {
      return (this->*gCommands[i].mFunc)(ac, av);
    }
  }

  mWindow->WriteFormatted("*** Invalid command(%s)\n", mCommand);
  return -1;
}
