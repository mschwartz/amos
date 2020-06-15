#include <FileSystems/BFileSystem.h>
#include <Exec/BTask.h>
#include <Exec/ExecBase.h>

class SimpleFileSystem;

class FileSystemTask : public BTask {
public:
  FileSystemTask(SimpleFileSystem *aFileSystem) : BTask("filesystem.task") {
    mFileSystem = aFileSystem;
  }
  ~FileSystemTask() {
    // should only happen if filesystem is unmounted
    bochs;
  }

public:
  void Run();

protected:
  SimpleFileSystem *mFileSystem;
};

void FileSystemTask::Run() {
  dlog("FileSystemTask Alive!\n");
  // read root sector
  while (1) {
    Sleep(1000);
  }
}

SimpleFileSystem::SimpleFileSystem(const char *aName) : BNode(aName) {
  gExecBase.AddTask(new FileSystemTask(this));
}
