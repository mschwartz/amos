#include "disk-tool.h"
#include "RawDisk.h"

const char *RAWFILE = "fs.raw";

const int MAX_ARGS = 100;

struct TOptions {
  char *filename;
  TUint64 size;
  TBool parent;
  char *command;
  char *args[MAX_ARGS];
  TOptions() {
    filename = strdup(RAWFILE);
    size = 10 * 1024 * 1024;
    parent = EFalse;
    command = ENull;
    for (int i = 0; i < MAX_ARGS; i++) {
      args[i] = ENull;
    }
  }
};

int help(const char *av0) {
  printf("Usage:\n");
  printf("  %s [options] command arguments\n", av0);
  printf("WHere\n");
  printf("  command may be one of:\n");
  printf("  * format\n");
  printf("  * ls\n");
  printf("  * mkdir path\n");
  printf("\n");
  printf("  and options may be any of:\n");
  printf("  * -f image_filename\n");
  printf("  * -s image_size (in bytes)\n");
  printf("  * -p (create parent directories for mkdir and copy)\n");
  return -1;
}

int main(int ac, char *av[]) {
  const char *opts = "f:s:p";
  TOptions options;

  int opt;
  while ((opt = getopt(ac, av, opts)) != -1) {
    switch(opt) {
      case 'f':
        options.filename = strdup(optarg);
        break;
      case 's':
        options.size = atol(optarg);
        break;
      case 'p':
        options.parent = ETrue;
        break;
      default:
        return help(av[0]);
        break;
    }
  }

  if (optind < ac) {
    options.command = strdup(av[optind]);
  }
  optind++;
  if (options.command == ENull) {
    return help(av[0]);
  }

  for (int i = 0; optind < ac; optind++, i++) {
    options.args[i] = strdup(av[optind]);
  }

  RawDisk raw(options.filename, options.size);

  const char *cmd = options.command;
  printf("COMMAND(%s)\n", cmd);
  if (strcasecmp(cmd, "FORMAT") == 0) {
    raw.Format();
    raw.Write();
    printf("Formatted %s\n", RAWFILE);
  }
  else if (strcasecmp(cmd, "LS") == 0) {
    raw.Read();
    raw.ListDirectory("/");
  }
  else if (strcasecmp(cmd, "MKDIR") == 0) {
    if (options.args[0] == ENull) {
      printf("*** mkdir requires path argument\n");
      return help(av[0]);
    }
    raw.Read();
    raw.MakeDirectory(options.args[0], options.parent);
    raw.Write();
  }
  else {
    printf("*** Invalid command '%s'\n", cmd);
    return help(av[0]);
  }

  return 0;
}
