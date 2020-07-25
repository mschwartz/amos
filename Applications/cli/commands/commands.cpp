#include "commands.hh"

struct cli_commands  gCommands[] = {
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
