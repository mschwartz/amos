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
  DirectorySector *GetDirectorySector(TUint64 mLba) {
    return (DirectorySector *)Sector(mLba);
  }
  TUint8 *AllocSector();
  void ReleaseSector(FreeSector *aSector);

  /**
   * Allocate 3 sectors: 1 for the directory/name, and 1 each for . and ..
   * The sectors are linked execept for the one with name, which caller
   * must link in the right place.
   */
  DirectorySector *AllocDirectory(const char *aName);
  DirectorySector *AllocFile(const char *aName);

public:
  /**
   * Format the disk image
   */
  TBool Format(const char *aVolumeName);

  /**
   * Find Directory/File node in specified directory (or /)
   */
  DirectorySector *Find(const char *aPath, DirectorySector *aDirectory = ENull);

  /**
   * Find Directory/File node walking specified path recursively
   */
  DirectorySector *FindPath(const char *aPath);

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

  /**
   * Type file to stdout
   */
  TBool TypeFile(const char *aPath);

public:
  static const char *AbsolutePath(const char *aRelativePath);
  void ListDirectory(const char *aPath);
  void DumpRootSector();

protected:
  char *mFilename;
  TUint64 mSize;
  TUint8 *mData;
  RootSector *mRootSector;
};

#endif
