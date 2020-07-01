#ifndef AMOS_SIMPLEFILESYSTEM_H
#define AMOS_SIMPLEFILESYSTEM_H

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
 *
 * NOTE: This is not necessarily intended to be the fastest or most robust FS designed.
 */

#include <Types.h>
#include <Exec/BFileSystem.h>

class SimpleFileSystem : public BFileSystem {
public:
  SimpleFileSystem(const char *aDiskDevice, TUint64 aUnit, TUint64 aRootLba);

protected:
  const char *mDiskDevice;
  TUint64 mUnit;
  TUint64 mRootLba;
};

#endif
