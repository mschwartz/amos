#include "cli.hh"
#include "commands/commands.hh"

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

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

TInt64 CliTask::Error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  char buf[512];
  vsprintf(buf, fmt, args);
  dputs(buf);
  va_end(args);

  mWindow->BeginPaint();
  mWindow->WriteFormatted("*** Error: %s\n", buf);
  mWindow->EndPaint();
  return -1;
}
/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

// repl

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
    switch (c) {
      case -1:
        continue;
      case 8: // backspace
        dlog("backspace!\n");
        if (ptr != &mCommand[0]) {
          *ptr-- = ' ';
          mWindow->BeginPaint();
          mWindow->Left();
          mWindow->Write(" ");
          mWindow->Left();
          mWindow->EndPaint();
        }
        break;
      case 13:
      case 10:
        *ptr = '\0';
        mWindow->BeginPaint();
        mWindow->Write("\n");
        mWindow->EndPaint();
        return;
      default:
        if (c >= 0x20 && c <= 0x7f) {
          *ptr++ = c;
          mWindow->BeginPaint();
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
