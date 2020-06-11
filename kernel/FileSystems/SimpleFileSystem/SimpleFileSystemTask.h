#ifndef SIMPLEFILESYSTEM_TASK_H
#define SIMPLEFILESYSTEM_TASK_H

#include <Devices/AtaDevice.h>
#include <Exec/BTask.h>
#include <Exec/ExecBase.h>
#include <Exec/Types/BSparseArray.h>

class SimpleFileSystem;

class SimpleFileSystemTask : public BTask {
public:
  SimpleFileSystemTask(SimpleFileSystem *aFileSystem, const char *aDiskDevice, TUint64 aUnit, TUint64 aRootLba)
      : BTask("filesystem.task"), mDiskDevice(aDiskDevice) {
    mFileSystem = aFileSystem;
    mUnit = aUnit;
    mRootLba = aRootLba;
  }
  ~SimpleFileSystemTask() {
    // should only happen if filesystem is unmounted
    bochs;
  }

public:
  void Run();

protected:
  /**
   * Return pointer to cached sector by LBA.  If not in cache, it is read from disk and added to cache.
   */
  void *Sector(TUint64 aLba);

  /**
   * Find Directory/File node in specified directory (or /)
   */
  DirectorySector *Find(const char *aPath, DirectorySector *aDirectory = ENull);

  /**
   * Find Directory/File node walking specified path recursively
   */
  DirectorySector *FindPath(const char *aPath);

protected:
  void OpenDirectory(FileSystemMessage *f);
  void ReadDirectory(FileSystemMessage *f);
  void CloseDirectory(FileSystemMessage *f);
  void MakeDirectory(FileSystemMessage *f);
  void RemoveDirectory(FileSystemMessage *f);
  void OpenFile(FileSystemMessage *f);
  void ReadFile(FileSystemMessage *f);
  void WriteFile(FileSystemMessage *f);
  void CloseFile(FileSystemMessage *f);
  void RemoveFile(FileSystemMessage *f);

protected:
  SimpleFileSystem *mFileSystem;
  const char *mDiskDevice;
  TUint64 mUnit;
  TUint64 mRootLba;
  RootSector mRootSector;
  BSparseArray *mDiskCache;
  // BAvlTree mDiskCache;
  MessagePort *mAtaReplyPort, *mAtaPort;
  AtaMessage *mAtaMessage;
  EFileSystemError mError;
};

#endif
