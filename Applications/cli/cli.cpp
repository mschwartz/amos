#include "CLI.hh"

class CliWindow : public BConsoleWindow {
public:
  CliWindow() : BConsoleWindow("Test Cli Window", 900, 700, 640, 480) {}
};

CliTask::CliTask() : BProcess("CliTask") {
  mPrompt = DuplicateString(" %s > ");
  CopyString(mCurrentDirectory, "/");
}

CliTask::~CliTask() {
  delete[] mPrompt;
  mPrompt = ENull;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

typedef TInt64 (CliTask::*TCommand)(TInt ac, char *av[]);

static struct {
  const char *mName;
  TCommand mFunc;
  const char *mHelp;
} sCommands[] = {
  "help", &CliTask::command_help, "print help (for command)",
  "pwd", &CliTask::command_pwd, "print cwd",
  "lsahci", &CliTask::command_lsahci, "list ahci table",
  "lspci", &CliTask::command_lspci, "list pci devices",
  "sysinfo", &CliTask::command_sysinfo, "print system information",
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

TInt64 CliTask::command_help(TInt ac, char **av) {
  mWindow->WriteFormatted("Help\n");
  for (TInt i = 0; sCommands[i].mFunc != ENull; i++) {
    mWindow->WriteFormatted("%-10s %s\n", sCommands[i].mName, sCommands[i].mHelp);
  }
  return 0;
}

TInt64 CliTask::command_pwd(TInt ac, char **av) {
  mWindow->WriteFormatted("pwd argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}

TInt64 CliTask::command_lsahci(TInt ac, char **av) {
  mWindow->WriteFormatted("lsahci argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}

TInt64 CliTask::command_lspci(TInt ac, char **av) {
  mWindow->WriteFormatted("lspci argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}

TInt64 CliTask::command_sysinfo(TInt ac, char **av) {
  mWindow->WriteFormatted("sysinfo argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}

TInt64 CliTask::command_date(TInt ac, char **av) {
  mWindow->WriteFormatted("date argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}

TInt64 CliTask::command_uptime(TInt ac, char **av) {
  mWindow->WriteFormatted("uptime argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}

TInt64 CliTask::command_df(TInt ac, char **av) {
  mWindow->WriteFormatted("df argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}

TInt64 CliTask::command_ps(TInt ac, char **av) {
  mWindow->WriteFormatted("ps argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}

TInt64 CliTask::command_theme(TInt ac, char **av) {
  mWindow->WriteFormatted("theme argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}

TInt64 CliTask::command_ls(TInt ac, char **av) {
  mWindow->WriteFormatted("ls argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}

TInt64 CliTask::command_touch(TInt ac, char **av) {
  mWindow->WriteFormatted("touch argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}

TInt64 CliTask::command_cat(TInt ac, char **av) {
  mWindow->WriteFormatted("cat argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}

TInt64 CliTask::command_less(TInt ac, char **av) {
  mWindow->WriteFormatted("less argc(%d)\n", ac);
  for (TInt i = 0; i < ac; i++) {
    mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  }
  return 0;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

TInt64 CliTask::ExecuteCommand(char *aCommand) {
  // mWindow->BeginPaint();
  // mWindow->WriteFormatted("Execute(%s)\n", aCommand);
  // mWindow->EndPaint();
  dlog("Command(%s)\n", aCommand);

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

  for (TInt i = 0; sCommands[i].mFunc != ENull; i++) {
    if (CompareStrings(av[0], sCommands[i].mName) == 0) {
      return (this->*sCommands[i].mFunc)(ac, av);
    }
  }

  mWindow->WriteFormatted("*** Invalid command(%s)\n", mCommand);
  return -1;
}

void CliTask::PrintPrompt() {
  mWindow->BeginPaint();
  mWindow->WriteFormatted(mPrompt, mCurrentDirectory);
  mWindow->EndPaint();
}

void CliTask::ReadCommand() {
  PrintPrompt();
  char *ptr = &mCommand[0];
  for (;;) {
    TInt c = mWindow->ReadKey();
    mWindow->BeginPaint();
    switch (c) {
      case -1:
        continue;
      case 13:
      case 10:
        *ptr = '\0';
        mWindow->Write("\n");
        mWindow->EndPaint();
        return;
      default:
        if (c >= 0x20 && c <= 0x7f) {
          *ptr++ = c;
          mWindow->Write(c);
          mWindow->EndPaint();
        }
        else {
          dlog("key: %02x\n", c);
        }
    }
  }
}

TInt64 CliTask::Run() {
  Sleep(5);
  BScreen *screen = mInspirationBase.FindScreen();
  mWindow = new CliWindow();
  screen->AddWindow(mWindow);

  for (;;) {
    // print prompt and read command
    ReadCommand();
    mWindow->BeginPaint();
    ExecuteCommand(mCommand);
    mWindow->EndPaint();
    // execute command
  }
  return 0;
}
