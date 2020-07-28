#include <FileSystems/SimpleFileSystem/SimpleFileSystem.hpp>
#include <FileSystems/SimpleFileSystem/SimpleFileSystemTask.hpp>
#include <Exec/ExecBase.hpp>

SimpleFileSystem::SimpleFileSystem(const char *aDiskDevice, TUint64 aUnit, TUint64 aRootLba)
    : BFileSystem("SimpleFileSystem"),
      mDiskDevice(DuplicateString(aDiskDevice)),
      mUnit(aUnit),
      mRootLba(aRootLba) {
  dlog("SimpleFileSystem(%s) - mUnit(%d) mRootLba(%d)\n", aDiskDevice, mUnit, mRootLba);
  gExecBase.AddTask(new SimpleFileSystemTask(this, mDiskDevice, mUnit, mRootLba));
}
