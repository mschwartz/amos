#include <Exec/Devices/ScreenDevice.h>
#include <Exec/ExecBase.h>

ScreenDevice::ScreenDevice() : BDevice() {
  ExecBase::GetExecBase().AddDevice(this);
}

ScreenDevice::~ScreenDevice() {
}

