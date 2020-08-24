#include <Exec/BDevice.hpp>

#define MAX_DEVICES 64

/**
  * BDevice
  */
BDevice::BDevice(const char *aNodeName) : BNodePri(aNodeName) {
}

BDevice::~BDevice() {
}

/**
  * BDeviceList
  */
DeviceList::DeviceList() : BListPri("Device List") {
//  bochs
//  dprint("construct device list\n");
}

DeviceList::~DeviceList() {
}

BDevice *DeviceList::FindDevice(const char *aName) {
  return (BDevice *)Find(aName);
}

void DeviceList::AddDevice(BDevice& aDevice) {
  dlog("\nAdd Device %x\n", &aDevice);
}

