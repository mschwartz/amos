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
#include <Exec/Types/BList.h>
#include <Exec/Types/BAvlTree.h>
#include <Exec/MessagePort.h>

#ifdef KERNEL
#include <Exec/x86/bochs.h>
#else
#include <stdio.h>
#define dlog printf
#endif

const TInt FILESYSTEM_SECTOR_SIZE = 512;
const TInt FILESYSTEM_NAME_MAXLEN = 255;

// these are compatible with Linux/Unix
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

typedef struct {
  TUint64 mLba,
    mLbaNext,  // next sector in chain (file data, directory, etc.)
    mLbaOwner; // parent / sector "owning" this one
  //
  void Dump() {
    dlog("     mLba: %d\n", mLba);
    dlog(" mLbaNext: %d\n", mLbaNext);
    dlog("mLbaOwner: %d\n", mLbaOwner);
  }
} PACKED BaseSector;

typedef struct {
  TUint64 mLbaRoot, // LBA of / (root of filesystem)
    mLbaHeap,       // LBA of first free sector on disk
    mLbaFree,       // LBA of first free sector in free list
    mLbaMax;        // LBA of last sector on disk
  TUint64 mUsed;    // count of used bytes
  TUint64 mFree;    // count of free bytes
  TUint64 mBlocksUsed, mBlocksFree;
  TUint64 mType; // file system type (Ext2, Ext3, Ext4, FAT, AMOS, ...)
  char mVolumeName[FILESYSTEM_NAME_MAXLEN + 1];
  TUint8 mPad[512 - (sizeof(TUint64) * 9) - FILESYSTEM_NAME_MAXLEN - 1];
  //
  // this is basically the output of our custom "df" command
  //
  void Dump() {
    dlog("RootSector %s\n", mVolumeName);
    dlog("%d bytes used (%d) / %d bytes free (%d)\n", mUsed, mBlocksUsed, mFree, mBlocksFree);
    dlog("\n");
    dlog("  mLbaRoot: %d\n", mLbaRoot);
    dlog("  mLbaHeap: %d\n", mLbaHeap);
    dlog("  mLbaFree: %d\n", mLbaFree);
    dlog("  mLbaMax: %d\n", mLbaMax);
    dlog("\n");
  }
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
  TUint8 mPad[512 - sizeof(BaseSector) - sizeof(TUint64) - sizeof(DirectoryStat) - FILESYSTEM_NAME_MAXLEN - 1];
  // void Stat();
  void Dump() {
    dlog("Directory Sector %s\n", mFilename);
    BaseSector::Dump();
    dlog(" mLbaFirst: %d\n", mLbaFirst);
    dlog("\n");
  }
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

// BAvlNode.mKey is the LBA of the sector
// cast the sector member to the appropriate *Sector above
struct CachedSector : public BAvlNode {
  CachedSector(TUint64 aLba) : BAvlNode(aLba) {
    mLru = 0;
  }
  TUint64 mLru; // least recently used timestamp
  TUint8 mSector[512];
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

struct FileSystemDescriptor : public BBase {
public:
  FileSystemDescriptor() { Reuse(); }

public:
  TBool IsFile() { return mDirectorySector->mStat.mMode & S_IFREG; }
  TBool IsDirectory() { return mDirectorySector->mStat.mMode & S_IFDIR; }

public:
  DirectorySector *mDirectorySector;
  DataSector *mDataSector;
  TUint64 mDataIndex; // index into current data sector
  TUint64 mFilePosition;

public:
  void Reuse() {
    mDirectorySector = ENull;
    mDataSector = ENull;
    mDataIndex = 0;
    mFilePosition = 0;
  }

public:
  void Dump() {
    dlog("FileSystemDescriptor %x\n", this);
    dlog("  mDirectorySector: %x\n", mDirectorySector);
    dlog("  mDataSector: %x\n", mDataSector);
    dlog("  mDataIndex: %d\n", mDataIndex);
    dlog("  mFilePosition: %d\n", mFilePosition);
  }
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

#if 0
class DirectoryDescriptor : public BNode {
public:
  DirectoryDescriptor(const char *aPath);

protected:
  DirectorySector *mDirectoryEntry;
};
#endif

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

class BFileSystem : public BNode {
public:
  BFileSystem(const char *aName);
  ~BFileSystem();
};

class BFileSystemList : public BList {
};

/********************************************************************************
 ********************************************************************************
 *******************************************************************************/

enum EFileSystemError {
  EFileSystemErrorNone,
  EFileSystemErrorNotFound,
  EFileSystemErrorNotADirectory,
  EFileSystemErrorNotAFile,
  EFileSystemErrorInvalidPath,
  EFileSystemErrorEndOfFile,
  EFileSystemErrorFileNotOpen,
};

enum EFileSystemCommand {
  EFileSystemInvalidCommand,
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
    mBuffer = ENull;
    Reuse(aCommand);
  }
  FileSystemMessage() : BMessage(ENull) {
    mBuffer = ENull;
    Reuse(EFileSystemOpen);
  }

public:
  void Reuse(EFileSystemCommand aCommand) {
    mCommand = aCommand;
    mError = EFileSystemErrorNone;
    mFlags = 0;
    mCount = 0;
    mDescriptor.Reuse();
  }

public:
  EFileSystemCommand mCommand;
  EFileSystemError mError;
  FileSystemDescriptor mDescriptor;
  TAny *mBuffer; // filename, read buffer, write buffer
  TInt64 mCount; // bytes to read/write, returned number of bytes actually read/written
  TInt64 mFlags;

public:
  void Dump() {
    dlog("FileSystemMessage %x\n", this);
    dlog("  mCommand: %d\n", mCommand);
    dlog("  mError: %d\n", mError);
    dlog("  mCount: %d\n", mCount);
    dlog("  mFlags: %d\n", mFlags);
    dlog("  mBuffer: %x\n", mBuffer);
    mDescriptor.Dump();
  }
};

#endif
