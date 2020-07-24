#include "CLI.hh"

class CliWindow : public BConsoleWindow {
public:
  CliWindow() : BConsoleWindow("Test Cli Window", 900, 700, 640, 480) {}
};

CliTask::CliTask() : BProcess("CliTask") {
  mPrompt = DuplicateString("> ");
  CopyString(mCurrentDirectory, "/");
}

CliTask::~CliTask() {
  delete[] mPrompt;
  mPrompt = ENull;
}

TInt64 CliTask::ExecuteCommand(char *aCommand) {
  mWindow->BeginPaint();
  mWindow->WriteFormatted("Execute(%s)\n", aCommand);
  mWindow->EndPaint();
  dlog("Command(%s)\n", aCommand);
  return 0;
}

void CliTask::ReadCommand() {
  mWindow->BeginPaint();
  mWindow->Write(mPrompt);
  mWindow->EndPaint();
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
    ExecuteCommand(mCommand);
    // execute command
  }
  return 0;
}
