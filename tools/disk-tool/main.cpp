#include "disk-tool.h"
#include "RawDisk.h"

const char *RAWFILE = "fs.raw";

const int MAX_ARGS = 8192;

struct TOptions {
  char *filename;
  TUint64 size;
  TBool parent;
  TBool recurse;
  char *command;
  TInt arg_count;
  char *args[MAX_ARGS];
  TOptions() {
    filename = strdup(RAWFILE);
    size = 10 * 1024 * 1024;
    parent = EFalse;
    command = ENull;
    arg_count = 0;
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
  printf("  * format [volume_name]\n");
  printf("  * ls\n");
  printf("  * df\n");
  printf("  * mkdir path\n");
  printf("  * cat path\n");
  printf("  * cp path source_file\n");
  printf("\n");
  printf("  and options may be any of:\n");
  printf("  * -f image_filename\n");
  printf("  * -s image_size (in bytes)\n");
  printf("  * -p (create parent directories for mkdir and copy)\n");
  printf("  * -r recursive (TBD)\n");
  return -1;
}

int main(int ac, char *av[]) {
  const char *opts = "f:s:pr";
  TOptions options;

  int opt;
  while ((opt = getopt(ac, av, opts)) != -1) {
    switch (opt) {
      case 'f':
        options.filename = strdup(optarg);
        break;
      case 's':
        options.size = atol(optarg);
        break;
      case 'p':
        options.parent = ETrue;
        break;
      case 'r':
        options.recurse = ETrue;
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
    options.arg_count++;
    options.args[i] = strdup(av[optind]);
    // printf("%d %s\n", i, options.args[i]);
  }
  // printf("arg_count = %d\n", options.arg_count);

  RawDisk raw(options.filename, options.size);

  const char *cmd = options.command;
  /********************************************************************************
   ********************************************************************************
   *******************************************************************************/

  if (strcasecmp(cmd, "FORMAT") == 0) {
    raw.Format(options.args[0] ? options.args[0] : "Untitled");
    raw.Write();
    printf("Formatted %s\n", RAWFILE);
  }

  /********************************************************************************
   ********************************************************************************
   *******************************************************************************/

  else if (strcasecmp(cmd, "LS") == 0) {
    raw.Read();
    raw.ListDirectory(options.args[0] ? options.args[0] : "/");
  }

  /********************************************************************************
   ********************************************************************************
   *******************************************************************************/

  else if (strcasecmp(cmd, "DF") == 0) {
    raw.Read();
    raw.DumpRootSector();
  }

  /********************************************************************************
   ********************************************************************************
   *******************************************************************************/

  else if (strcasecmp(cmd, "CAT") == 0) {
    raw.Read();
    if (!raw.TypeFile(options.args[0])) {
      return 1;
    }
  }

  /********************************************************************************
   ********************************************************************************
   *******************************************************************************/

  else if (strcasecmp(cmd, "CP") == 0) {
    raw.Read();
    if (options.arg_count < 2) {
      return help(av[0]);
    }
    
    char dest_fn[8192], host_fn[8192];
    TInt ndx_d = options.arg_count - 1;
    const char *dst_pos = options.args[ndx_d--];
    printf("dst_pos(%s)\n", dst_pos);

    DirectorySector *d = raw.FindPath(dst_pos);
    if (!d) {
      printf("*** Invalid path (%s)\n", dst_pos);
      return -1;
    }

    printf("dst_pos(%s) %o\n", dst_pos, d->mStat.mMode);
    if ((d->mStat.mMode & S_IFDIR) != 0) {
      // copy to directory
      for (TInt i = 0; i <= ndx_d; i++) {
        char *src = strdup(options.args[i]),
             *host_path = strdup(options.args[i]),
             *dst_file = src,
             *slash = strrchr(src, '/');

        printf("src(%s) host_path(%s) dst_file(%s) slash(%s)\n", src, host_path, dst_file, slash);
        if (slash) {
          *slash = '\0';
          dst_file = ++slash;
        }
        // printf("host_path(%s) src(%s) dst_file(%s)\n", host_path, src, dst_file);

        sprintf(dest_fn, "%s/%s", dst_pos, dst_file);
        printf("cp %s %s\n", host_path, dest_fn);
        raw.CopyFile(dest_fn, host_path);
      }
    }
    else {
      printf("raw.CopyFile(%s, %s)\n", options.args[1], options.args[0]);
      raw.CopyFile(options.args[1], options.args[0]);
    }
    raw.Write();
  }

  /********************************************************************************
   ********************************************************************************
   *******************************************************************************/

  else if (strcasecmp(cmd, "MKDIR") == 0) {
    if (options.args[0] == ENull) {
      printf("*** mkdir requires path argument\n");
      return help(av[0]);
    }
    raw.Read();
    raw.MakeDirectory(options.args[0], options.parent);
    raw.Write();
  }

  /********************************************************************************
   ********************************************************************************
   *******************************************************************************/

  else {
    printf("*** Invalid command '%s'\n", cmd);
    return help(av[0]);
  }

  return 0;
}
