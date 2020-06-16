#ifndef AMOS_FILESYSTEMS_BFILESYSTEM_H
#define AMOS_FILESYSTEMS_BFILESYSTEM_H

// TODO: support file system across multiple devices

/**
 * Minimally there is a RootSector and a Directory Sectors for /, /., and /.. 
 *
 * The sector immediately following is our heap.  As we need sectors to allocate for writing to disk,
 * they are simply allocated from the heap.  This can help make files contiguous sectors on disk.
 *
 * When a file is deleted, it sectors are added to the free list.
 *
 * When the heap is fully allocated, sectors will be allocated from the free list.
 */

#include <Exec/Types.h>
#include <Exec/BList.h>
#include <Exec/BAvlTree.h>
#include <Exec/MessagePort.h>

const TInt FILESYSTEM_SECTOR_SIZE = 512;
const TInt FILESYSTEM_NAME_MAXLEN = 255;

const TUint64 S_IFMT = 0170000;   // bit mask for the file type bit field
const TUint64 S_IFSOCK = 0140000; // socket
const TUint64 S_IFLNK = 0120000;  // symbolic link
const TUint64 S_IFREG = 0100000;  // regular file
const TUint64 S_IFBLK = 0060000;  // block device
const TUint64 S_IFDIR = 0040000;  // directory
const TUint64 S_IFCHR = 0020000;  // character device
const TUint64 S_IFIFO = 0010000;  // FIFO
//
const TUint64 S_ISUID = 04000; // set user id bit
const TUint64 S_ISGID = 04000; // set user group id bit
const TUint64 S_ISVTX = 01000; // sticky bit
//
const TUint64 S_IRUSR = 0400;
const TUint64 S_IWUSR = 0200;
const TUint64 S_IXUSR = 0100;
const TUint64 S_IRWXU = (S_IRUSR | S_IWUSR | S_IXUSR);
//
const TUint64 S_IRGRP = 0040;
const TUint64 S_IWGRP = 0020;
const TUint64 S_IXGRP = 0010;
const TUint64 S_IRWXG = (S_IRGRP | S_IWGRP | S_IXGRP);
//
const TUint64 S_IROTH = 0004;
const TUint64 S_IWOTH = 0002;
const TUint64 S_IXOTH = 0001;
const TUint64 S_IRWXO = (S_IROTH | S_IWOTH | S_IXOTH);

struct BaseSector {
  TUint64 mLba,
    mLbaNext,
    mLbaOwner;
};

typedef struct {
  TUint64 mLbaRoot, // LBA of / (root of filesystem)
    mLbaHeap,       // LBA of first free sector on disk
    mLbaFree,       // LBA of first free sector in free list
    mLbaMax;        // LBA of last sector on disk
  TUint64 mUsed,    // count of used bytes
    mFree;          // count of free bytes
  TUint64 mBlocksUsed, mBlocksFree;
  TUint64 mType; // file system type (Ext2, Ext3, Ext4, FAT, AMOS, ...)
  char mVolumeName[FILESYSTEM_NAME_MAXLEN + 1];
} PACKED RootSector;

typedef struct {
  TUint64 mMode;
  TUint64 mNumLinks;
  TUint64 mOwner;
  TUint64 mOwnerGroup;
  TUint64 mSize;
  TUint64 mAccessTime, // last time file data was accessed
    mModifiedTime,     // last time file data was modified
    mChangeTime;       // last time file status changed
} PACKED DirectoryStat;

struct DirectorySector : public BaseSector {
  TUint64 mLbaFirst; // LBA of file data on disk or first DirectorySector in subdir

  char mFilename[FILESYSTEM_NAME_MAXLEN + 1];
  DirectoryStat mStat;
} PACKED;

struct DataSector : public BaseSector {
  TUint8 mData[FILESYSTEM_SECTOR_SIZE - sizeof(BaseSector)];
} PACKED;

struct FreeSector : public BaseSector {
  TUint8 mData[FILESYSTEM_SECTOR_SIZE - sizeof(BaseSector)];
} PACKED;

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

struct CachedRootSector : public BAvlNode {
  TUint64 mLru; // least recently used timestamp
  RootSector sector;
};

struct CachedDirectorySector : public BAvlNode {
  TUint64 mLru; // least recently used timestamp
  DirectorySector sector;
};

struct CachedDataSector : public BAvlNode {
  TUint64 mLru; // least recently used timestamp
  DataSector sector;
};

class FileDescriptor : public BNode {
public:
  FileDescriptor(const char *aFilename);
  ~FileDescriptor();

public:
  TBool Alive() { return mAlive; }

public:
  TBool Truncate();
  TBool Open();
  TInt64 Read(TAny *aBuffer, TUint64 aSize);
  TInt64 Write(TAny *aBuffer, TUint64 aSize);
  TUint64 Seek(TUint64 aFilePosition);
  TBool Close();
  TBool Delete();
  TBool Stat(DirectoryStat &aStatBuf);

public:
  TUint64 Size() {
    return mDirectorySector.mStat.mSize;
  }

protected:
  TBool mAlive;
  DirectorySector mDirectorySector;
  TUint64 mFilePosition;
};

class DirectoryDescriptor : public BNode {
public:
  DirectoryDescriptor(const char *aPath);

public:
  TBool Stat(DirectoryStat &aStatBuf);

protected:
  DirectorySector *mFirstDirectoryEntry;
  DirectorySector *mDirectoryEntry;
};

class BFileSystem : public BNode {
public:
  BFileSystem(const char *aName) : BNode(aName) {}
};

class BFileSystemList : public BList {
};

enum EFileSystemError {
  EFileSystemErrorNone,
};

enum EFileSystemCommand {
  EFileSystemOpenDirectory,
  EFileSystemReadDirectory,
  EFileSystemCloseDirectory,
  EFileSystemMakeDirectory,
  EFileSystemRemoveDirectory,
  EFileSystemOpen,
  EFileSystemRead,
  EFileSystemWrite,
  EFileSystemClose,
  EFileSystemRemoveFile,
};

class FileSystemMessage : public BMessage {
public:
  FileSystemMessage(MessagePort *aReplyPort, EFileSystemCommand aCommand) : BMessage(aReplyPort) {
    mError = EFileSystemErrorNone;
  }

public:
  EFileSystemCommand mCommand;
  EFileSystemError mError;
  TAny *mBuffer; // filename, read buffer, write buffer
  TInt64 mCount; // bytes to read/write, returned number of bytes actually read/written
};

class SimpleFileSystem : public BFileSystem {
public:
  SimpleFileSystem(const char *aDiskDevice, TUint64 aUnit, TUint64 aRootLba);

protected:
  const char *mDiskDevice;
  TUint64 mUnit;
  TUint64 mRootLba;
};

#endif
