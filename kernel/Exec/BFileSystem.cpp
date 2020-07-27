#include <Exec/BFileSystem.hpp>
#include <Devices/AtaDevice.hpp>
#include <Exec/BTask.hpp>
#include <Exec/ExecBase.hpp>

BFileSystem::BFileSystem(const char *aName) : BNode(aName) {
  //
}

BFileSystem::~BFileSystem() {
  //
}
