#include "RawDisk.h"

RawDisk::RawDisk(const char *aFilename, TUint64 aSize) {
  mFilename = strdup(aFilename);
  mSize = aSize;
  mData = new TUint8[mSize];
  bzero(mData, mSize);

  mRootSector = (RootSector *)Sector(0);
}

RawDisk::~RawDisk() {
  delete[] mData;
  delete[] mFilename;
}

static void RootSectorDump(TAny *root) {
  RootSector *s = (RootSector *)root;
  printf("Root Sector %s\n", s->mVolumeName);
  printf("%d bytes used (%d) / %d bytes free (%d)\n", s->mUsed, s->mBlocksUsed, s->mFree, s->mBlocksFree);
  printf("\n");
  printf("  mLbaRoot: %d\n", s->mLbaRoot);
  printf("  mLbaHeap: %d\n", s->mLbaHeap);
  printf("  mLbaFree: %d\n", s->mLbaFree);
  printf("  mLbaMax: %d\n", s->mLbaMax);
  printf("\n");
}

static void DirectorySectorDump(TAny *root) {
  DirectorySector *s = (DirectorySector *)root;
  printf("Directory Sector %s\n", s->mFilename);
  printf("  mLba: %d\n", s->mLba);
  // printf("  mLbaRoot: %d\n", s->mLbaRoot);
  // printf("  mLbaHeap: %d\n", s->mLbaHeap);
  // printf("  mLbaFree: %d\n", s->mLbaFree);
  // printf("  mLbaMax: %d\n", s->mLbaMax);
  printf("\n");
}

TBool RawDisk::Read() {
  // printf("Reading %d bytes from %s\n\n", mSize, mFilename);
  int fd = open(mFilename, O_RDONLY);
  if (fd < 0) {
    perror("Open");
    return EFalse;
  }
  size_t read_size = read(fd, mData, mSize);
  if (read_size != mSize) {
    printf("*** Read(%s) %d != %d\n", mFilename, read_size, mSize);
    return EFalse;
  }
  return ETrue;
}

TBool RawDisk::Write() {
  // printf("Writing %d bytes to %s\n", mSize, mFilename);
  int fd = open(mFilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (fd < 0) {
    perror("Open");
    return EFalse;
  }
  size_t size = write(fd, mData, mSize);
  // perror("WRITE");
  // printf("Wrote %d bytes\n", size);
  close(fd);
  return size == mSize;
}

TBool RawDisk::Format(const char *aVolumeName) {
  TUint64 timestamp = time(ENull);

  mRootSector->mBlocksUsed = 1;
  mRootSector->mLbaHeap = 2; // heap starts right after the root sector
  mRootSector->mLbaFree = 0;
  mRootSector->mLbaMax = mSize / FILESYSTEM_SECTOR_SIZE;
  mRootSector->mUsed = 1 * FILESYSTEM_SECTOR_SIZE;
  mRootSector->mFree = mSize - mRootSector->mUsed;
  mRootSector->mBlocksFree = mRootSector->mFree / FILESYSTEM_SECTOR_SIZE;
  strcpy(mRootSector->mVolumeName, aVolumeName);
  mRootSector->mType = 0;

  DirectorySector *slash = AllocDirectory("/");
  mRootSector->mLbaRoot = slash->mLba;

  RootSectorDump(mRootSector);
  DirectorySectorDump(slash);

  return ETrue;
}

// find file starting in spcified directory, processing . and ..
DirectorySector *RawDisk::Find(const char *aFilename, DirectorySector *aDirectory) {
  if (aDirectory == ENull) {
    aDirectory = (DirectorySector *)Sector(mRootSector->mLbaRoot);
  }
  aDirectory = GetDirectorySector(aDirectory->mLbaFirst);
  while (aDirectory) {
    if (strcmp(aDirectory->mFilename, aFilename) == 0) {
      return aDirectory;
    }
    aDirectory = aDirectory->mLbaNext
                   ? (DirectorySector *)Sector(aDirectory->mLbaNext)
                   : ENull;
  }
  return ENull;
}

DirectorySector *RawDisk::FindPath(const char *aPath) {
  DirectorySector *cwd = GetDirectorySector(mRootSector->mLbaRoot);
  const char *token = strtok((char *)aPath, "/");
  if (!token) {
    return cwd;
  }
  while (ETrue) {
    const char *next_token = strtok(NULL, "/");
    // printf("find %s %s\n", token, cwd->mFilename);
    DirectorySector *d = Find(token, cwd);
    if (!d) {
      if (next_token == ENull) {
        return cwd;
      }
      // invalid path
      printf("*** %s is not a directory\n", token);
      return ENull;
    }
    DirectoryStat *s = &d->mStat;
    if (s->mMode & S_IFDIR) {
      if (next_token) {
        cwd = GetDirectorySector(cwd->mLbaFirst);
      }
      else {
        return d;
      }
    }
    else if (s->mMode & S_IFREG && !next_token) {
      return d;
    }
    else {
      // not a directory, so it cannot be .../file/more...
      if (next_token) {
        printf("*** invalid path at %s/%s\n", token, next_token);
        return ENull;
      }
    }
    token = next_token;
  }
}

TBool RawDisk::TypeFile(const char *aPath) {
  DirectorySector *d = FindPath(aPath);
  if (!d) {
    printf("*** %s not found\n", aPath);
    return EFalse;
  }
  if (!(d->mStat.mMode & S_IFREG)) {
    printf("*** %s not a regular file\n", aPath);
    return EFalse;
  }
  TInt64 size = d->mStat.mSize;
  printf("cat %s %d bytes\n", d->mFilename, size);
  DataSector *f = (DataSector *)Sector(d->mLbaFirst);
  while (size > 0) {
    TInt64 count = MIN(sizeof(f->mData), size);
    for (TInt64 x = 0; x < count; x++) {
      putchar(f->mData[x]);
    }
    size -= count;
    if (f->mLbaNext == 0) {
      break;
    }
    f = (DataSector *)Sector(f->mLbaNext);
  }
  return ETrue;
}

TUint8 *RawDisk::AllocSector() {
  TUint64 lba;
  struct FreeSector *s;

  // first try to allocate from heap
  if (mRootSector->mLbaHeap != 0) {
    lba = mRootSector->mLbaHeap++;
    if (mRootSector->mLbaHeap > mRootSector->mLbaMax) {
      mRootSector->mLbaHeap = 0;
    }
  }
  else {
    // no heap left, try free list
    lba = mRootSector->mLbaFree;
    if (lba == 0) {
      // disk is full!
      return ENull;
    }
    // unlink first free sector from free list (keystone in mRootSector)
    s = (struct FreeSector *)Sector(lba);
    mRootSector->mLbaFree = s->mLbaNext;
  }

  // bookkeeping
  mRootSector->mUsed += FILESYSTEM_SECTOR_SIZE;
  mRootSector->mFree -= FILESYSTEM_SECTOR_SIZE;
  mRootSector->mBlocksUsed++;
  mRootSector->mBlocksFree--;

  // set mLba in returned sector to lba
  s = (struct FreeSector *)Sector(lba);
  s->mLba = lba;
  return Sector(lba);
}

void RawDisk::ReleaseSector(FreeSector *aSector) {
  aSector->mLbaNext = mRootSector->mLbaFree;
  mRootSector->mLbaFree = aSector->mLba;
  mRootSector->mUsed -= FILESYSTEM_SECTOR_SIZE;
  mRootSector->mFree += FILESYSTEM_SECTOR_SIZE;
  mRootSector->mBlocksUsed;
  mRootSector->mBlocksFree++;
}

DirectorySector *RawDisk::AllocFile(const char *aName) {
  DirectorySector *n = (DirectorySector *)AllocSector();
  TUint64 timestamp = time(ENull);
  DirectoryStat *stat = &n->mStat;
  strcpy(n->mFilename, aName);
  stat->mMode = S_IFREG | 0644;
  stat->mNumLinks = 0;
  stat->mOwner = 0;
  stat->mOwnerGroup = 0;
  stat->mSize = 0;
  stat->mAccessTime = stat->mModifiedTime = stat->mChangeTime = timestamp;
  return n;
}
DirectorySector *RawDisk::AllocDirectory(const char *aName) {
  DirectorySector *n = (DirectorySector *)AllocSector(),
                  *dot = (DirectorySector *)AllocSector(),
                  *dotdot = (DirectorySector *)AllocSector();

  TUint64 timestamp = time(ENull);

  // set up dot
  {
    dot->mLbaOwner = n->mLba;
    dot->mLbaFirst = 0;
    dot->mLbaNext = dotdot->mLba;
    strcpy(dot->mFilename, ".");

    DirectoryStat *stat = &dot->mStat;
    stat->mMode = S_IFDIR | 0755;
    stat->mNumLinks = 0;
    stat->mOwner = 0;
    stat->mOwnerGroup = 0;
    stat->mSize = 0;
    stat->mAccessTime = stat->mModifiedTime = stat->mChangeTime = timestamp;
  }

  // set up dotdot
  {
    dotdot->mLbaOwner = n->mLba;
    dot->mLbaFirst = 0;
    strcpy(dotdot->mFilename, "..");
    dotdot->mLbaNext = 0;

    DirectoryStat *stat = &dotdot->mStat;
    stat->mMode = S_IFDIR | 0755;
    stat->mNumLinks = 0;
    stat->mOwner = 0;
    stat->mOwnerGroup = 0;
    stat->mSize = 0;
    stat->mAccessTime = stat->mModifiedTime = stat->mChangeTime = timestamp;
  }

  // set up the new directory
  {
    strcpy(n->mFilename, aName);
    n->mLbaOwner = 0; // caller needs to fill this in!
    n->mLbaFirst = dot->mLba;
    DirectoryStat *stat = &n->mStat;
    stat->mMode = S_IFDIR | 0755;
    stat->mNumLinks = 0;
    stat->mOwner = 0;
    stat->mOwnerGroup = 0;
    stat->mSize = 0;
    stat->mAccessTime = stat->mModifiedTime = stat->mChangeTime = timestamp;
  }

  return n;
}

// Create directory specified by aPath, which can be dir/dir/dir/... format.
// if aFlag is set, then any dir along the aPath will be created, otherwise
// the directory will not be created and this method will fail.
TBool RawDisk::MakeDirectory(const char *aPath, TBool aFlag) {
  const char *full_path = strdup(aPath);
  // printf("MakeDirectory(%s)\n", aPath);

  // walk path
  const char *token = strtok((char *)aPath, "/");
  // start cwd at /
  DirectorySector *cwd = (DirectorySector *)Sector(mRootSector->mLbaRoot);

  while (ETrue) {
    // we'll need next_token to determine if we're at the end of the path to create
    const char *next_token = strtok(NULL, "/");

    // find DirectorySector with name == token in cwd
    DirectorySector *d = Find(token, cwd);
    if (!d) {
      // not found
      if (aFlag || !next_token) {
        // printf(" create directory %s\n", token);

        // create the directory
        DirectorySector *n = AllocDirectory(token);
        if (!n) {
          printf("*** Failed to make directory %s\n", aPath);
          // failed to MakeDirectory
          delete[] full_path;
          return EFalse;
        }

        // insert at head of directory sector list of parent
        n->mLbaNext = cwd->mLbaFirst;
        cwd->mLbaFirst = n->mLba;
        delete[] full_path;
        return ETrue;
      }
      else {
        // could not create the directory, invalid path
        // like mkdir foo/bar/baz and bar does not exist
        delete[] full_path;
        return EFalse;
      }
    }
    else if (!d->mStat.mMode & S_IFDIR) {
      // path component is not a directory
      printf("*** %s is not a directory\n", d->mFilename);
      delete[] full_path;
      return EFalse;
    }
    if (next_token == ENull) {
      printf("*** %s exists\n", full_path);
      delete[] full_path;
      return EFalse;
    }
    // printf("%s found\n", token);
    cwd = d;
    token = next_token;
    if (!token) {
      delete[] full_path;
      return ETrue;
    }
  }
}

TBool RawDisk::RemoveDirectory(const char *aPath) {
  return ETrue;
}

TBool RawDisk::CopyFile(const char *aDestination, const char *aSource) {
  char *dest = strdup(aDestination);
  char *slash = strrchr(dest, '/');
  const char *path = dest,
             *fn = slash ? &slash[1] : dest;

  DirectorySector *d;
  if (slash) {
    // null terminate the path part
    *slash = '\0';

    char *base = strdup(path);
    d = FindPath(path);
    if (!d) {
      printf("*** path %s not found\n", base);
      delete[] base;
      return EFalse;
    }
  }
  else {
    d = GetDirectorySector(mRootSector->mLbaRoot);
  }

  if (Find(fn, d)) {
    printf("*** %s exists\n", fn);
    return EFalse;
  }
  DirectorySector *n = AllocFile(fn);
  n->mLbaOwner = d->mLba;
  n->mLbaNext = d->mLbaFirst;
  d->mLbaFirst = n->mLba;

  int fd = open(aSource, O_RDONLY);
  if (fd < 0) {
    printf("*** can't open %s\n", aSource);
    perror("open");
    return EFalse;
  }

  TInt64 size = lseek(fd, 0, 2);
  lseek(fd, 0, 0);

  n->mStat.mSize = size;
  if (!size) {
    n->mLbaFirst = 0;
    close(fd);
    return ETrue;
  }

  DataSector *f = (DataSector *)AllocSector();

  f->mLbaNext = 0;
  n->mLbaFirst = f->mLba;
  TInt64 count = read(fd, f->mData, sizeof(f->mData));
  if (count < 0) {
    perror("count");
    return EFalse;
  }
  size -= count;
  while (size > 0) {
    DataSector *ff = (DataSector *)AllocSector();
    ff->mLbaNext = 0;
    f->mLbaNext = ff->mLba;
    TInt64 count = read(fd, ff->mData, sizeof(ff->mData));
    if (count < 0) {
      perror("count");
      return EFalse;
    }
    f = ff;
    size -= count;
  }
  close(fd);
  return ETrue;
}

TBool RawDisk::RemoveFile(const char *aPath) {
  return ETrue;
}

static void format_mode(char *buf, TUint64 aMode) {
  buf[0] = (aMode & S_IFDIR ? 'd' : '-');
  buf[1] = (aMode & S_IRUSR ? 'r' : '-');
  buf[2] = (aMode & S_IWUSR ? 'w' : '-');
  buf[3] = (aMode & S_IXUSR ? 'x' : '-');
  buf[4] = (aMode & S_IRGRP ? 'r' : '-');
  buf[5] = (aMode & S_IWGRP ? 'w' : '-');
  buf[6] = (aMode & S_IXGRP ? 'x' : '-');
  buf[7] = (aMode & S_IROTH ? 'r' : '-');
  buf[8] = (aMode & S_IWOTH ? 'w' : '-');
  buf[9] = (aMode & S_IXOTH ? 'x' : '-');
  buf[10] = '\0';
}
static char *format_user(char *buf, TUint64 uid) {
  strcpy(buf, "root");
  return buf;
}
static char *format_group(char *buf, TUint64 gid) {
  strcpy(buf, "root");
  return buf;
}

void RawDisk::DumpRootSector() {
  RootSectorDump(mRootSector);
  printf("sizeof(BaseSector) = %d\n", sizeof(BaseSector));
  printf("sizeof(RootSector) = %d\n", sizeof(RootSector));
  printf("sizeof(DirectorySector) = %d\n", sizeof(DirectorySector));
  printf("sizeof(DataSector) = %d\n", sizeof(DataSector));
  printf("sizeof(DataSector.mData) = %d\n", sizeof(DataSector::mData));
  printf("sizeof(FreeSector) = %d\n", sizeof(FreeSector));
}

void RawDisk::ListDirectory(const char *aPath) {
  char mode[16], user[16], group[16], *path = strdup(aPath);
  DirectorySector *d = (DirectorySector *)FindPath(aPath);
  if (!d) {
    printf("No such directory %s\n", path);
    delete[] path;
    return;
  }
  if ((d->mStat.mMode & S_IFDIR) == 0) {
    DirectoryStat *s = &d->mStat;
    format_mode(mode, s->mMode);
    format_user(user, s->mOwner);
    format_group(group, s->mOwnerGroup);

    const time_t t = s->mModifiedTime;
    // unix style ls -l
    printf("%s %s %s %8d %s %5d %s\n",
      mode,
      user, group,
      s->mSize,
      d->mLba,
      ctime(&t),
      d->mFilename);
  }
  else {
    printf("\nDirectory of %s:\n", path);
    d = GetDirectorySector(d->mLbaFirst);

    while (ETrue) {
      DirectoryStat *s = &d->mStat;
      format_mode(mode, s->mMode);
      format_user(user, s->mOwner);
      format_group(group, s->mOwnerGroup);

      const time_t t = (time_t)s->mModifiedTime;
      char *tt = strdup(ctime(&t));

      char *ttt = &tt[4];
      ttt[15] = '\0';
      // unix style ls -l
      printf("%s %s %s %8d %5d %s %s\n",
        mode,
        user, group,
        s->mSize,
        d->mLba,
        ttt,
        d->mFilename);

      delete[] tt;
      if (d->mLbaNext == 0) {
        break;
      }
      d = (DirectorySector *)Sector(d->mLbaNext);
    }
  }
  delete[] path;
}

const char *RawDisk::AbsolutePath(const char *aRelativePath) {
  const char *ret = realpath(aRelativePath, ENull);
  perror("realpath");
  return ret;
}
