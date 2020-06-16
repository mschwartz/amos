#include <Exec/BFileSystem.h>
#include <Devices/AtaDevice.h>
#include <Exec/BTask.h>
#include <Exec/ExecBase.h>

FileDescriptor::FileDescriptor() {
  mDirectorySector = ENull;
}

BFileSystem::BFileSystem(const char *aName) : BNode(aName) {
  //
}

BFileSystem::~BFileSystem() {
  //
}
