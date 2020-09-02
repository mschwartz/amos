#ifndef AMOS_EXEC_TSYSINFO_HPP
#define AMOS_EXEC_TSYSINFO_HPP

#include <Types.hpp>

typedef struct {
  TUint64 mMillis;
  TUint64 mEBDA; // Extended BIOS Data Area
  TUint64 mRam;

  TUint64 mVideoMode;
  TAny *mDisplayFrameBuffer;
  TInt64 mDisplayWidth;
  TInt64 mDisplayHeight;
  TInt64 mDisplayDepth;
  TInt64 mDisplayPitch;

  TUint64 mBootSector;
  TUint64 mBootSectors;
  TUint64 mKernelSector;
  TUint64 mKernelSectors;
  TUint64 mRootSector;

  TUint64 mBootDrive;
  TUint64 mNumDrives;
  TUint64 mNumHeads;
  TUint64 mNumCylinders;
  TUint64 mNumSectors;
  TUint64 mDiskCX;

  TUint64 mCpuMhz;
  TUint64 mCpuCores;
  TUint64 mBochs;
  TUint64 mInitStart, mInitEnd;
  TUint64 mTextStart, mTextEnd;
  TUint64 mRoDataStart, mRoDataEnd;
  TUint64 mDataStart, mDataEnd;
  TUint64 mBssStart, mBssEnd;
  TUint64 mKernelEnd;
  TUint64 mDiskSize;
  char mVersion[64];

  void Dump() {
    // dlog("TSystemInfo(%x)\n", this);
    dlog("%s %s\n", mVersion, mBochs ? "BOCHS ENABLED" : "NO BOCHS");
    dlog("     CPU Speed: %0d Mhz\n", mCpuMhz);
    dlog("           RAM: %016x (%d)\n", mRam, mRam);
    dlog("          init: %016x - %016x\n", mInitStart, mInitEnd);
    dlog("          text: %016x - %016x\n", mTextStart, mTextEnd);
    dlog("        rodata: %016x - %016x\n", mRoDataStart, mRoDataEnd);
    dlog("          data: %016x - %016x\n", mDataStart, mDataEnd);
    dlog("           bss: %016x - %016x\n", mBssStart, mBssEnd);
    dlog("    kernel_end: %016x\n", mKernelEnd);
    dlog("          ebda: %016x\n", mEBDA);
    dlog("\n");
    dlog("  mDisplayWidth: %d\n", mDisplayWidth);
    dlog(" mDisplayHeight: %d\n", mDisplayHeight);
    dlog("  mDisplayDepth: %d\n", mDisplayDepth);
    dlog("  mDisplayPitch: %d\n", mDisplayPitch);
    dlog("  mFrameBuffer: %x\n", mDisplayFrameBuffer);
    dlog("\n");
    dlog("    mNumDrives: %d\n", mNumDrives);
    dlog("    mBootDrive: %02x\n", mBootDrive);
    dlog("       mDiskCX: %02x\n", mDiskCX);
    dlog("     mNumHeads: %d\n", mNumHeads);
    dlog(" mNumCylinders: %d\n", mNumCylinders);
    dlog("   mNumSectors: %d\n", mNumSectors);
    dlog("     mDiskSize: %d\n", mDiskSize);
    dlog("\n");
    dlog("   mBootSector: %d\n", mBootSector);
    dlog("  mBootSectors: %d\n", mBootSectors);
    dlog(" mKernelSector: %d\n", mKernelSector);
    dlog("mKernelSectors: %d\n", mKernelSectors);
    dlog("   mRootSector: %d\n", mRootSector);

    dprint("\n");
  }
} PACKED TSystemInfo;

static TSystemInfo &gSystemInfo = *(TSystemInfo *)0x5000; // see memory.inc

#endif
