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

TBool RawDisk::Read() {
  printf("Reading %d bytes from %s\n\n", mSize, mFilename);
  int fd = open(mFilename, O_RDONLY);
  if (fd < 0) {
    perror("Open");
    return EFalse;
  }
  size_t read_size = read(fd, mData, mSize);
  if (read_size != mSize) {
    printf("Read(%s) %d != %d\n", mFilename, read_size, mSize);
    return EFalse;
  }
  return ETrue;
}

TBool RawDisk::Write() {
  printf("Writing %d bytes to %s\n", mSize, mFilename);
  int fd = open(mFilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (fd < 0) {
    perror("Open");
    return EFalse;
  }
  size_t size = write(fd, mData, mSize);
  perror("WRITE");
  printf("Wrote %d bytes\n", size);
  close(fd);
  return size == mSize;
}

TBool RawDisk::Format() {
  mRootSector->mLbaRoot = 1;
  mRootSector->mLbaHeap = 3;
  mRootSector->mLbaFree = 0;
  mRootSector->mLbaMax = mSize / FILESYSTEM_SECTOR_SIZE;
  mRootSector->mUsed = 3 * FILESYSTEM_SECTOR_SIZE;
  mRootSector->mFree = mSize - mRootSector->mUsed;

  DirectorySector *dot = (DirectorySector *)Sector(1);
  dot->mLba = 1;
  dot->mLbaOwner = 1;
  dot->mLbaFirst = 0;
  dot->mLbaNext = 2; // lba of dotdot
  strcpy(dot->mFilename, ".");

  DirectoryStat *stat = &dot->mStat;
  stat->mMode = S_IFDIR | 0755;
  stat->mNumLinks = 0;
  stat->mOwner = 0;
  stat->mOwnerGroup = 0;
  stat->mSize = 0;
  stat->mAccessTime = stat->mModifiedTime = stat->mChangeTime = time(ENull);

  DirectorySector *dotdot = (DirectorySector *)Sector(2);
  dotdot->mLba = 2;
  dotdot->mLbaOwner = dot->mLba;
  dotdot->mLbaFirst = 0;
  dotdot->mLbaNext = 0; // lba of dotdot
  strcpy(dotdot->mFilename, "..");

  stat = &dotdot->mStat;
  stat->mMode = S_IFDIR | 0755;
  stat->mNumLinks = 0;
  stat->mOwner = 0;
  stat->mOwnerGroup = 0;
  stat->mSize = 0;
  stat->mAccessTime = stat->mModifiedTime = stat->mChangeTime = time(ENull);

  return ETrue;
}

// find file starting in spcified directory, processing . and ..
DirectorySector *RawDisk::Find(const char *aFilename, DirectorySector *aDirectory) {
  if (aDirectory == ENull) {
    aDirectory = (DirectorySector *)Sector(mRootSector->mLbaRoot);
  }
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

TUint8 *RawDisk::AllocSector() {
  TUint64 lba;
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
    FreeSector *s = (FreeSector *)Sector(lba);
    mRootSector->mLbaFree = s->mLbaNext;
  }

  // bookkeeping
  mRootSector->mUsed += 512;
  mRootSector->mFree -= 512;

  // set mLba in returne4d sector to lba
  FreeSector *s = (FreeSector *)Sector(lba);
  s->mLbaNext = lba;
  return Sector(lba);
}

DirectorySector *RawDisk::MakeDirectory(const char *aName, DirectorySector *aParent) {
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
    strcpy(dot->mFilename, ".");
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
    n->mLbaOwner = aParent->mLba;
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

TBool RawDisk::MakeDirectory(const char *aPath, TBool aFlag) {
  printf("MakeDirectory(%s)\n", aPath);
  // walk path
  const char *token = strtok((char *)aPath, "/");

  DirectorySector *cwd = (DirectorySector *)Sector(mRootSector->mLbaRoot);

  while (ETrue) {
    // we'll need next_token to determine if we're at the end of the path to create
    const char *next_token = strtok(NULL, "/");

    // find DirectorySector with name == token in cwd
    DirectorySector *d = Find(token, cwd);
    if (!d) {
      printf("%s not found\n", token);
      // not found
      if (aFlag || !next_token) {
        printf(" create directory %s\n", token);

        // create the directory
        DirectorySector *n = MakeDirectory(token, cwd);
        if (!n) {
          printf("Failed to make directory %s\n", aPath);
          // failed to MakeDirectory
          return EFalse;
        }
        // insert at head of directory sector list of parent
        n->mLbaNext = cwd->mLbaNext;
        cwd->mLbaNext = n->mLba;
        return ETrue;
      }
      else {
        // could not create the directory, invalid path
        // like mkdir foo/bar/baz and bar does not exist
        return EFalse;
      }
    }
    else if (!d->mStat.mMode & S_IFDIR) {
      // path component is not a directory
      return EFalse;
    }
    printf("%s found\n", token);
    cwd = d;
    token = next_token;
    if (!token) {
      return ETrue;
    }
  }
}

TBool RawDisk::RemoveDirectory(const char *aPath) {
  return ETrue;
}

TBool RawDisk::CopyFile(const char *aDestination, const char *aSOurce) {
  return ETrue;
}

TBool RawDisk::RemoveFile(const char *aPath) {
  return ETrue;
}

void RootSectorDump(TAny *root) {
  RootSector *s = (RootSector *)root;
  printf("Root Sector %d used / %d free\n", s->mUsed, s->mFree);
  printf("  mLbaRoot: %d\n", s->mLbaRoot);
  printf("  mLbaHeap: %d\n", s->mLbaHeap);
  printf("  mLbaFree: %d\n", s->mLbaFree);
  printf("  mLbaMax: %d\n", s->mLbaMax);
}

void RawDisk::ListDirectory(const char *aPath) {
  RootSectorDump(mRootSector);
  printf("\nDirectory:\n");
  DirectorySector *d = (DirectorySector *)Sector(mRootSector->mLbaRoot);
  while (ETrue) {
    DirectoryStat *s = &d->mStat;
    printf("%5d %-16.16s %d %d:%d %o\n",
      d->mLba,
      d->mFilename, s->mSize,
      s->mOwner, s->mOwnerGroup,
      s->mMode);
    if (d->mLbaNext == 0) {
      break;
    }
    d = (DirectorySector *)Sector(d->mLbaNext);
  }
}

const char *RawDisk::AbsolutePath(const char *aRelativePath) {
  const char *ret = realpath(aRelativePath, ENull);
  perror("realpath");
  return ret;
}
