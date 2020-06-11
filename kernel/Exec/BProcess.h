#ifndef EXEC_BPROCESS_H
#define EXEC_BPROCESS_H

/**
 * BProcess is a heavier weight version of BTask.  It has:
 * * FileSystem abilities and FileSystem related things.
 * * Resource Tracking for FileSystem
 */
#include <Exec/BTask.h>
#include <Exec/BFileSystem.h>

struct FileDescriptor : public BNode {
  FileDescriptor(const char *aName);
  ~FileDescriptor();
  FileSystemMessage mMessage;

  TBool IsFile() { return mMessage.mDescriptor.IsFile(); }
  TBool IsDirectory() { return mMessage.mDescriptor.IsDirectory(); }
  const DirectoryStat *Stat() { return &mMessage.mDescriptor.mDirectorySector->mStat; }
  const char *Filename() { return mMessage.mDescriptor.mDirectorySector->mFilename; }
public:
  void Dump() {
    dlog("FileDescriptor %x\n", this);
    dlog("  mMessage: %x\n", mMessage);
    mMessage.Dump();
  }
};

class BProcess : public BTask {
public:
  BProcess(const char *aName, TInt64 aPri = 0, TUint64 aStackSize = default_task_stack_size);
  virtual ~BProcess();

public:
  FileDescriptor *OpenDirectory(const char *aName);
  TBool ReadDirectory(FileDescriptor *aFileDescriptor);
  TBool CloseDirectory(FileDescriptor *aFileDescriptor);

public:
  FileDescriptor *OpenFile(const char *aName);
  TBool ReadFile(FileDescriptor *aFileDescriptor, TAny *aBuffer, TUint64 aSize);
  TBool WriteFile(FileDescriptor *aFileDescriptor, TAny *aBuffer, TUint64 aSize);
  TBool CloseFile(FileDescriptor *aFileDescriptor);
  TBool RemoveFile(FileDescriptor *aFileDescriptor);

protected:
  /**
   * Send fsMessage to FileSystem and wait for reply
   */
  TBool DoIO(FileDescriptor *aFileDescriptor);

protected:
  MessagePort *mFsReplyPort;
  BList mFileDescriptorList;
};

#endif
