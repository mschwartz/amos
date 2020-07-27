#include "commands.hpp"

TInt64 CliTask::command_sysinfo(TInt ac, char **av) {
  TSystemInfo s;
  gExecBase.GetSystemInfo(&s);
  mWindow->WriteFormatted("%s %s\n", s.mVersion, s.mBochs ? "BOCHS ENABLED" : "NO BOCHS");
  mWindow->WriteFormatted("     CPU Speed: %0d Mhz\n", s.mCpuMhz);
  mWindow->WriteFormatted("          init: %016x - %016x\n", s.mInitStart, s.mInitEnd);
  mWindow->WriteFormatted("          text: %016x - %016x\n", s.mTextStart, s.mTextEnd);
  mWindow->WriteFormatted("        rodata: %016x - %016x\n", s.mRoDataStart, s.mRoDataEnd);
  mWindow->WriteFormatted("          data: %016x - %016x\n", s.mDataStart, s.mDataEnd);
  mWindow->WriteFormatted("           bss: %016x - %016x\n", s.mBssStart, s.mBssEnd);
  mWindow->WriteFormatted("    kernel_end: %016x\n", s.mKernelEnd);
  mWindow->WriteFormatted("          ebda: %016x\n", s.mEBDA);
  mWindow->WriteFormatted("\n");
  mWindow->WriteFormatted("       Display: width(%d) height(%d) depth(%d) pitch(%d)\n",
    s.mDisplayWidth, s.mDisplayHeight, s.mDisplayDepth, s.mDisplayPitch);
  mWindow->WriteFormatted("  mFrameBuffer: %x\n", s.mDisplayFrameBuffer);
  mWindow->WriteFormatted("\n");
  mWindow->WriteFormatted("    mNumDrives: %d\n", s.mNumDrives);
  mWindow->WriteFormatted("    mBootDrive: %02x\n", s.mBootDrive);
  mWindow->WriteFormatted("       mDiskCX: %02x\n", s.mDiskCX);
  mWindow->WriteFormatted(" mNumCylinders: %d\n", s.mNumCylinders);
  mWindow->WriteFormatted("     mNumHeads: %d\n", s.mNumHeads);
  mWindow->WriteFormatted("   mNumSectors: %d\n", s.mNumSectors);
  mWindow->WriteFormatted("     mDiskSize: %d\n", s.mDiskSize);
  mWindow->WriteFormatted("\n");
  mWindow->WriteFormatted("   mBootSector: %d\n", s.mBootSector);
  mWindow->WriteFormatted("  mBootSectors: %d\n", s.mBootSectors);
  mWindow->WriteFormatted(" mKernelSector: %d\n", s.mKernelSector);
  mWindow->WriteFormatted("mKernelSectors: %d\n", s.mKernelSectors);
  mWindow->WriteFormatted("   mRootSector: %d\n", s.mRootSector);

  return 0;
}
