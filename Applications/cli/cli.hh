#ifndef AMOS_EXAMPLES_CLI_HH
#define AMOS_EXAMPLES_CLI_HH

#include <Exec/ExecBase.h>
#include <Inspiration/InspirationBase.h>
#include "cliwindow.hh"

class CliTask : public BProcess {
public:
  CliTask();
  ~CliTask();

public:
  TInt64 Run();

protected:
  TInt64 Error(const char *fmt, ...);
  void PrintPrompt();
  void ReadCommand();
  TInt64 ExecuteCommand(char *aCommand);

public:
  TInt64 command_help(TInt ac, char *av[]);
  TInt64 command_pwd(TInt ac, char *av[]);
  TInt64 command_lsahci(TInt ac, char *av[]);
  TInt64 command_lspci(TInt ac, char *av[]);
  TInt64 command_sysinfo(TInt ac, char *av[]);
  TInt64 command_date(TInt ac, char *av[]);
  TInt64 command_uptime(TInt ac, char *av[]);
  TInt64 command_df(TInt ac, char *av[]);
  TInt64 command_ps(TInt ac, char *av[]);
  TInt64 command_theme(TInt ac, char *av[]);
  // files
  TInt64 command_ls(TInt ac, char *av[]);
  TInt64 command_touch(TInt ac, char *av[]);
  TInt64 command_cat(TInt ac, char *av[]);
  TInt64 command_less(TInt ac, char *av[]);

protected:
  char *mPrompt;
  CliWindow *mWindow;
  char mCommand[512];
  char mCurrentDirectory[2048];
  // TODO history
};

#endif
