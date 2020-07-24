#ifndef AMOS_EXAMPLES_CLI_HH
#define AMOS_EXAMPLES_CLI_HH

#include <Exec/ExecBase.h>
#include <Inspiration/InspirationBase.h>

class CliWindow;

class CliTask : public BProcess {
public:
  CliTask();
  ~CliTask();

public:
  TInt64 Run();

protected:
  TInt64 ExecuteCommand(char *aCommand);
  void ReadCommand();
  
protected:
  char *mPrompt;
  CliWindow *mWindow;
  char mCommand[512];
  char mCurrentDirectory[2048];
  // TODO history
};

#endif
