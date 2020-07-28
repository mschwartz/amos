#ifndef SIMPLEFILESYSTEM_TASK_H
#define SIMPLEFILESYSTEM_TASK_H

#include <Exec/ExecBase.hpp>
#include <Types/BSparseArray.hpp>
#include <Exec/BTask.hpp>
#include <Devices/AtaDevice.hpp>

class SimpleFileSystem;

class SimpleFileSystemTask : public BTask {
public:
  SimpleFileSystemTask(SimpleFileSystem *aFileSystem, const char *aDiskDevice, TUint64 aUnit, TUint64 aRootLba);
  ~SimpleFileSystemTask() {
    // should only happen if filesystem is unmounted
    bochs;
  }

public:
  TInt64 Run();

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
  MessagePort *mAtaReplyPort, *mAtaPort;
  AtaMessage *mAtaMessage;
  EFileSystemError mError;
};

#endif
