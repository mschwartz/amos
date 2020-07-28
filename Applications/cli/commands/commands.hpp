#ifndef AMOS_APPLICATIONS_CLI_COMMANDS_H
#define AMOS_APPLICATIONS_CLI_COMMANDS_H

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

#include "../cli.hpp"

typedef TInt64 (CliTask::*TCommand)(TInt ac, char *av[]);

struct cli_commands {
  const char *mName;
  TCommand mFunc;
  const char *mHelp;
} ;

extern struct cli_commands gCommands[];

#endif
