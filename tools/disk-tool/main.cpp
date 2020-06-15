#include "disk-tool.h"
#include "RawDisk.h"

#include <libgen.h>

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

    char dstfn[8192], srcfn[8192];
    TInt ndx_d = options.arg_count - 1;

    char *dstdir = options.args[ndx_d--],
         *dstpath = strdup(dstdir);

    DirectorySector *d = raw.FindPath(dstdir);
    delete[] dstdir;

    if (!d) {
      printf("*** Invalid path (%s)\n", dstpath);
      return -1;
    }

    if ((d->mStat.mMode & S_IFDIR) != 0) {
      // copy to directory
      for (TInt i = 0; i <= ndx_d; i++) {
        char *hostpath = strdup(options.args[i]),
             *srcfile = strdup(basename(hostpath)),
             *dstfile = strdup(basename(dstpath));

        if (strcmp(d->mFilename, dstfile) != 0) {
          sprintf(dstfn, "%s", dstpath);
        }
        else {
          sprintf(dstfn, "%s/%s", dstpath, srcfile);
        }
        printf("cp %-64.64s => %s\n", hostpath, dstfn);
        raw.CopyFile(dstfn, hostpath);

	delete[] dstfile;
	delete[] srcfile;
	delete[] hostpath;
      }
    }
    else {
      printf("raw.CopyFile(%s, %s)\n", dstpath, options.args[0]);
      raw.CopyFile(dstpath, options.args[0]);
    }
    delete [] dstpath;
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
