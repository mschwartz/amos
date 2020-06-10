#ifndef RAWDISK_H
#define RAWDISK_H

#include "disk-tool.h"

class RawDisk {
public:
  RawDisk(const char *aFilename, TUint64 aSize);
  ~RawDisk();

public:
  /**
   * Read Raw disk image from mFilename
   */
  TBool Read();

  /**
   * Write Raw disk image to mFilename
   */
  TBool Write();

protected:
  TUint8 *Sector(TUint64 mLba) {
    return &mData[mLba * 512];
  }
  TUint8 *AllocSector();
  DirectorySector *MakeDirectory(const char *aName, DirectorySector *aParent);

public:
  /**
   * Format the disk image
   */
  TBool Format();

  /**
   * Find Directory/File node following path;
   */
  DirectorySector *Find(const char *aPath, DirectorySector *aDirectory = ENull);

  /**
   * Make directory specified by aPath.  
   * Makes parent dirs if aFlag is ETrue
   */
  TBool MakeDirectory(const char *aPath, TBool aFlag = EFalse);

  /**
   * Remove directory specified by aPath.  
   */
  TBool RemoveDirectory(const char *aPath);

  /**
   * Copy file aSource in the host filesystem to aDestination path
   * in disk image.
   */
  TBool CopyFile(const char *aDestination, const char *aSource);

  /**
   * Remove file from the disk image.
   */
  TBool RemoveFile(const char *aFilename);

public:
  static const char *AbsolutePath(const char *aRelativePath);
  void ListDirectory(const char *aPath);

protected:
  char *mFilename;
  TUint64 mSize;
  TUint8 *mData;
  RootSector *mRootSector;
};

#endif
