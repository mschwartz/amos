#include <FileSystems/SimpleFileSystem/SimpleFileSystem.h>
#include <FileSystems/SimpleFileSystem/SimpleFileSystemTask.h>
#include <Exec/ExecBase.h>

SimpleFileSystem::SimpleFileSystem(const char *aDiskDevice, TUint64 aUnit, TUint64 aRootLba)
    : BFileSystem("SimpleFileSystem"),
      mDiskDevice(DuplicateString(aDiskDevice)),
      mUnit(aUnit),
      mRootLba(aRootLba) {
  dlog("SimpleFileSystem(%s) - mUnit(%d) mRootLba(%d)\n", aDiskDevice, mUnit, mRootLba);
  gExecBase.AddTask(new SimpleFileSystemTask(this, mDiskDevice, mUnit, mRootLba));
}
