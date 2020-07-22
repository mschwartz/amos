#include <FileSystems/SimpleFileSystem/SimpleFileSystem.h>
#include <FileSystems/SimpleFileSystem/SimpleFileSystemTask.h>

#define DEBUGME
#undef DEBUGME

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

// constructor
SimpleFileSystemTask::SimpleFileSystemTask(SimpleFileSystem *aFileSystem,
  const char *aDiskDevice,
  TUint64 aUnit,
  TUint64 aRootLba)
    : BTask("filesystem.task"), mDiskDevice(aDiskDevice) {
  mFileSystem = aFileSystem;
  mUnit = aUnit;
  mRootLba = aRootLba;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

void *SimpleFileSystemTask::Sector(TUint64 aLba) {
  // first look in sector cache

  CachedSector *fs = (CachedSector *)mDiskCache->Find(aLba);
  if (fs == ENull) {
#ifdef DEBUGME
    dlog("cache miss\n");
#endif
    fs = new CachedSector(aLba);
    mAtaMessage->mCommand = EAtaReadBlocks;
    mAtaMessage->mLba = aLba + mRootLba;
    mAtaMessage->mBuffer = &fs->mSector[0];
    mAtaMessage->mCount = 1;
    mAtaMessage->mError = EAtaErrorNone;
    mAtaMessage->mReplyPort = mAtaReplyPort;

    mAtaMessage->Send(mAtaPort);

    WaitPort(mAtaReplyPort);
    mDiskCache->Add((BSparseArrayNode &)*fs);
  }
  else {
#ifdef DEBUGME
    dlog("cache hit %x\n", fs);
#endif
  }

  fs->mLru++;
  return &fs->mSector;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

DirectorySector *SimpleFileSystemTask::Find(const char *aFilename, DirectorySector *aDirectory) {
  if (aDirectory == ENull) {
    aDirectory = (DirectorySector *)Sector(mRootSector.mLbaRoot);
  }
  aDirectory = (DirectorySector *)Sector(aDirectory->mLbaFirst);
  while (aDirectory) {
    if (CompareStrings(aDirectory->mFilename, aFilename) == 0) {
      return aDirectory;
    }
    aDirectory = aDirectory->mLbaNext
                   ? (DirectorySector *)Sector(aDirectory->mLbaNext)
                   : ENull;
  }
  return ENull;
}

DirectorySector *SimpleFileSystemTask::FindPath(const char *aPath) {
  char path[4096], *xpath = &path[0];
  CopyString(path, aPath);

  DirectorySector *cwd = (DirectorySector *)Sector(mRootSector.mLbaRoot);
  if (xpath[0] == '/') {
    xpath++;
  }

  if (*xpath == '\0') {
    return cwd;
  }

  char token[256];

  xpath = GetToken(xpath, token, '/');

  if (xpath == ENull) {
    return cwd;
  }

  for (;;) {
    char next_token[256];
    next_token[0] = '\0';
    xpath = GetToken(xpath, next_token, '/');
    DirectorySector *d = Find(token, cwd);

    if (!d) {
      if (next_token[0] == '\0') {
        mError = EFileSystemErrorNotADirectory;
        return ENull;
      }
    }

    DirectoryStat *s = &d->mStat;
    // DirectoryStat::Dump(s, d->mFilename);
    if (s->mMode & S_IFDIR) {
      if (next_token[0] != '\0') {
        cwd = (DirectorySector *)Sector(cwd->mLbaFirst);
        // cwd->Dump();
      }
      else {
        return d;
      }
    }
    else if (s->mMode & S_IFREG && next_token[0] == '\0') {
      return d;
    }
    else {
      // not a directory, so it cannot be .../file/more...
      if (next_token[0] != '\0') {
        mError = EFileSystemErrorInvalidPath;
        dlog("*** invalid path at %s/%s\n", token, next_token);
        return ENull;
      }
    }
    CopyString(token, next_token);
  }
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

void SimpleFileSystemTask::OpenDirectory(FileSystemMessage *f) {
  DirectorySector *d = FindPath((char *)f->mBuffer);
  f->mDescriptor.mDirectorySector = d;
  f->mDescriptor.mDataIndex = -1;
  f->mError = mError;
}

void SimpleFileSystemTask::ReadDirectory(FileSystemMessage *f) {
  DirectorySector *d = (DirectorySector *)f->mDescriptor.mDirectorySector;
  if (!d) {
    f->mError = EFileSystemErrorNotADirectory;
  }
  else if (f->mDescriptor.mDataIndex == -1 && d->mLbaFirst) {
    // dlog("ReadDirectory(%s)\n", d->mFilename);
    DirectorySector *dd = (DirectorySector *)Sector(d->mLbaFirst);
    f->mDescriptor.mDirectorySector = dd;
    f->mDescriptor.mDataIndex++;
  }
  else if (d->mLbaNext != 0) {
    d = (DirectorySector *)Sector(d->mLbaNext);
    f->mDescriptor.mDirectorySector = d;
    f->mDescriptor.mDataIndex++;
  }
  else {
    f->mError = EFileSystemErrorEndOfFile;
    // bochs;
  }
}

void SimpleFileSystemTask::CloseDirectory(FileSystemMessage *f) {
  // nothing to do?
}

void SimpleFileSystemTask::MakeDirectory(FileSystemMessage *f) {}
void SimpleFileSystemTask::RemoveDirectory(FileSystemMessage *f) {}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

void SimpleFileSystemTask::OpenFile(FileSystemMessage *f) {
  DirectorySector *d = FindPath((char *)f->mBuffer);
  f->mDescriptor.mDirectorySector = d;
  f->mError = mError;
  f->mDescriptor.mDataSector = d->mLbaFirst ? (DataSector *)Sector(d->mLbaFirst) : ENull;
  f->mDescriptor.mDataIndex = 0;
  f->mDescriptor.mFilePosition = 0;
}

void SimpleFileSystemTask::ReadFile(FileSystemMessage *f) {
  TUint64 actual = 0;

  FileSystemDescriptor *file = (FileSystemDescriptor *)&f->mDescriptor;
  if (file->mDirectorySector == ENull) {
    dlog("  *** file not open\n");
    f->mError = EFileSystemErrorFileNotOpen;
    f->mCount = actual;
    return;
  }

  if (file->mFilePosition > file->mDirectorySector->mStat.mSize ||
      file->mDataSector == ENull) {
    f->mError = EFileSystemErrorEndOfFile;
    f->mCount = actual;
    return;
  }

  TUint8 *dst = (TUint8 *)f->mBuffer;

  TUint64 size = file->mDirectorySector->mStat.mSize,
    bufsize = f->mCount;;

  TInt64 count;
  for (count = 0; count < f->mCount; count++) {
    if (file->mFilePosition >= size) {
      break;
    }

    if (file->mDataIndex > bufsize) {
      if (file->mDataSector->mLbaNext == 0) {
        // end of file
        file->mDataSector = ENull;
        return;
      }
      // next sector
      file->mDataSector = (DataSector *)Sector(file->mDataSector->mLbaNext);
      file->mDataIndex = 0;
    }

    *dst++ = file->mDataSector->mData[file->mDataIndex++];
    file->mFilePosition++;
    actual++;
  }
  // set actual count read
  f->mCount = actual;
}

void SimpleFileSystemTask::WriteFile(FileSystemMessage *f) {}

void SimpleFileSystemTask::CloseFile(FileSystemMessage *f) {
  FileSystemDescriptor *file = (FileSystemDescriptor *)&f->mDescriptor;
  file->mDirectorySector = ENull;
}

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

void SimpleFileSystemTask::RemoveFile(FileSystemMessage *f) {}

TInt64 SimpleFileSystemTask::Run() {
  dprint("\n");
  dlog("SimpleFileSystemTask Run!\n");

  mDiskCache = new BSparseArray(4096);
  // create message port to get requests from applications
  MessagePort *msgPort = CreateMessagePort("simple.filesystem");
  gExecBase.AddMessagePort(*msgPort);

  // we send read/write requests messages to mDiskDevice(ada.device) message port
  mAtaPort = ENull;
  while (!mAtaPort) {
    dlog("  Finding port(%s)\n", mDiskDevice);
    Forbid();
    mAtaPort = gExecBase.FindMessagePort(mDiskDevice);
    Permit();
    if (!mAtaPort) {
      dlog("  Port(%s) not found, sleeping\n", mDiskDevice);
      Sleep(1);
    }
  }
  dlog("  found ata port %x\n", mAtaPort);

  // we get replies to our messages at our private replyPort
  mAtaReplyPort = CreateMessagePort();

  // we need to read the root sector
  dlog("  SimpleFileSystemTask read sector %d from unit %d\n", 9, mUnit);
  mAtaMessage = AtaMessage::CreateReadMessaage(mAtaReplyPort, mUnit, 9, &this->mRootSector, 1);
  dlog("  mAtaMessage created %x\n", mAtaMessage);

  CopyMemory(&this->mRootSector, Sector(0), 512);

  this->mRootSector.Dump();

  for (;;) {
    WaitPort(msgPort);
    while (FileSystemMessage *f = (FileSystemMessage *)msgPort->GetMessage()) {
      f->mError = EFileSystemErrorNone;
      switch (f->mCommand) {
        case EFileSystemOpenDirectory:
          OpenDirectory(f);
          break;
        case EFileSystemReadDirectory:
          ReadDirectory(f);
          break;
        case EFileSystemCloseDirectory:
          CloseDirectory(f);
          break;
        case EFileSystemMakeDirectory:
          MakeDirectory(f);
          break;
        case EFileSystemRemoveDirectory:
          RemoveDirectory(f);
          break;
        case EFileSystemOpen:
          OpenFile(f);
          break;
        case EFileSystemRead:
          ReadFile(f);
          break;
        case EFileSystemWrite:
          WriteFile(f);
          break;
        case EFileSystemClose:
          CloseFile(f);
          break;
        case EFileSystemRemoveFile:
          RemoveFile(f);
          break;
        default:
	  dprint("\n\n");
          dlog("*** SimpleFileSystem: unknown command %d\n", f->mCommand);
	  dprint("\n\n");
          bochs;
      }
      f->Reply();
    }
  }
}
