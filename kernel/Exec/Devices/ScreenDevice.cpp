#include <Exec/Devices/ScreenDevice.h>

ScreenDevice::ScreenDevice() : BDevice() {
  gDeviceList.AddDevice(*this);
}

ScreenDevice::~ScreenDevice() {
}

