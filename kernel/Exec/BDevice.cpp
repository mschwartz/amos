#include <Exec/BDevice.h>
#include <x86/bochs.h>

#define MAX_DEVICES 16

BDeviceList gDeviceList;

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
  dprintf("Find device %s\n", aName);
  return ENull;
}

void BDeviceList::AddDevice(BDevice& aDevice) {
  dprintf("Add Device %x\n", &aDevice);
}

