#include <FileSystems/BFileSystem.h>
#include <Devices/AtaDevice.h>
#include <Exec/BTask.h>
#include <Exec/ExecBase.h>

class SimpleFileSystem;

class SimpleFileSystemTask : public BTask {
public:
  SimpleFileSystemTask(SimpleFileSystem *aFileSystem, const char *aDiskDevice, TUint64 aUnit, TUint64 aRootLba)
      : BTask("filesystem.task"), mDiskDevice(aDiskDevice) {
    mFileSystem = aFileSystem;
    mUnit = aUnit;
    mRootLba = aRootLba;
  }
  ~SimpleFileSystemTask() {
    // should only happen if filesystem is unmounted
    bochs;
  }

public:
  void Run();

protected:
  TBool ReadSector(TUint64 aLba, TAny *aDest);

protected:
  SimpleFileSystem *mFileSystem;
  const char *mDiskDevice;
  TUint64 mUnit, mRootLba;
  RootSector mRootSector;
  BAvlTree mDiskCache;
};

TBool SimpleFileSystemTask::ReadSector(TUint64 aLba, TAny *aDest) {
}

void SimpleFileSystemTask::Run() {
  dlog("SimpleFileSystemTask Alive!\n");

  // create message port to get requests from applications
  MessagePort *msgPort = CreateMessagePort("simple.filesystem");
  gExecBase.AddMessagePort(*msgPort);

  // we send read/write requests messages to mDiskDevice(ada.device) message port
  MessagePort *ataPort = gExecBase.FindMessagePort(mDiskDevice);

  // we get replies to our messages at our private replyPort
  MessagePort *replyPort = CreateMessagePort();

  // we need to read the root sector
  dlog("SimpleFileSystemTask read sector %d from unit %d\n", mUnit, mRootLba);
  AtaMessage *m = AtaMessage::CreateReadMessaage(replyPort, mUnit, mRootLba, &this->mRootSector, 1);
  m->SendMessage(ataPort);
  WaitPort(replyPort);
  // mRootSector has been read in!
  dlog("  Volume(%s)\n", mRootSector.mVolumeName);
  dlog("  used/free: %d/%d\n", mRootSector.mUsed, mRootSector.mFree);
  dlog("  mLbaRoot: %d\n", mRootSector.mLbaRoot);
  dlog("  mLbaHeap: %d\n", mRootSector.mLbaHeap);
  dlog("  mLbaFree: %d\n", mRootSector.mLbaFree);
  dlog("   mLbaMax: %d\n", mRootSector.mLbaMax);

  while (ETrue) {
    WaitPort(msgPort);
    while (FileSystemMessage *f = (FileSystemMessage *)msgPort->GetMessage()) {
      switch (f->mCommand) {
        case EFileSystemOpenDirectory:
          break;
        case EFileSystemReadDirectory:
          break;
        case EFileSystemCloseDirectory:
          break;
        case EFileSystemMakeDirectory:
          break;
        case EFileSystemRemoveDirectory:
          break;
        case EFileSystemOpen:
          break;
        case EFileSystemRead:
          break;
        case EFileSystemWrite:
          break;
        case EFileSystemClose:
          break;
        case EFileSystemRemoveFile:
          break;
      }
      m->ReplyMessage();
    }
  }
}

SimpleFileSystem::SimpleFileSystem(const char *aDiskDevice, TUint64 aUnit, TUint64 aRootLba)
    : BFileSystem("SimpleFileSystem"),
      mDiskDevice(DuplicateString(aDiskDevice)),
      mUnit(aUnit),
      mRootLba(aRootLba) {
  dlog("SimpleFileSystem(%s) - mUnit(%d) mRootLba(%d)\n", aDiskDevice, mUnit, mRootLba);
  gExecBase.AddTask(new SimpleFileSystemTask(this, mDiskDevice, mUnit, mRootLba));
}
