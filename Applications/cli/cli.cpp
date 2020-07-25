#include "cli.hh"
#include "commands/commands.hh"

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

  for (TInt i = 0; gCommands[i].mFunc != ENull; i++) {
    if (CompareStrings(av[0], gCommands[i].mName) == 0) {
      return (this->*gCommands[i].mFunc)(ac, av);
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
