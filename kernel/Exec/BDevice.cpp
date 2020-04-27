#include <Exec/BDevice.h>
#include <x86/bochs.h>

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
BDeviceList::BDeviceList() : BListPri("Device List") {
//  bochs
//  dprint("construct device list\n");
}

BDeviceList::~BDeviceList() {
}

BDevice *BDeviceList::FindDevice(const char *aName) {
  return (BDevice *)Find(aName);
}

void BDeviceList::AddDevice(BDevice& aDevice) {
  dprintf("\nAdd Device %x\n", &aDevice);
}

