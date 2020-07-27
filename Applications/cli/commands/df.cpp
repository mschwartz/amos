#include "commands.hpp"
#include <Exec/ExecBase.hpp>
#include <Exec/BFileSystem.hpp>

static void FormatNumber(char *aBuffer, TUint64 aNumber, TBool aHuman) {
  if (aHuman) {
    if (aNumber > TERRABYTE) {
      aNumber /= TERRABYTE;
      sprintf(aBuffer, "%dT", aNumber);
    }
    else if (aNumber > GIGABYTE) {
      aNumber /= GIGABYTE;
      sprintf(aBuffer, "%dG", aNumber);
    }
    else if (aNumber > MEGABYTE) {
      aNumber /= MEGABYTE;
      sprintf(aBuffer, "%dM", aNumber);
    }
    else if (aNumber > KILOBYTE) {
      aNumber /= KILOBYTE;
      sprintf(aBuffer, "%dK", aNumber);
    }
    else {
      sprintf(aBuffer, "%d", aNumber);
    }
  }
  else {
    sprintf(aBuffer, "%d", aNumber);
  }
}

TInt64 CliTask::command_df(TInt ac, char **av) {
  TBool human = EFalse;
  if (ac > 1 && CompareStrings(av[1], "-h") == 0) {
    human = ETrue;
  }

  mWindow->WriteFormatted("%-32s FSTYPE    %-10s    %-10s  %8s\n", "Volume Name", "Bytes Used", "Bytes Free", "Capacity");

  char used[64], free[64];

  for (BFileSystem *fs = gExecBase.FirstFileSystem(); !gExecBase.EndFileSystems(fs); fs = gExecBase.NextFileSystem(fs)) {
    RootSector *s = fs->mRootSector;
    FormatNumber(used, s->mUsed, human);
    FormatNumber(free, s->mFree, human);
    TInt pct = s->mUsed * 100 / s->mFree;
    mWindow->WriteFormatted("%-32s AMOSFS    %10s    %10s  %7d%%\n", s->mVolumeName, used, free, pct);
  }
  // mWindow->WriteFormatted("df argc(%d)\n", ac);
  // for (TInt i = 0; i < ac; i++) {
  //   mWindow->WriteFormatted("argv[%d] = (%s)\n", i, av[i]);
  // }
  return 0;
}
