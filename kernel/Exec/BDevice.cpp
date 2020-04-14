#include <Exec/BDevice.h>
#include <x86/bochs.h>

#define MAX_DEVICES 16

/**
  * BDevice
  */
BDevice::BDevice(const char *aName) : BNodePri(aName, 0) {
}

BDevice::~BDevice() {
}

/**
  * BDeviceList
  */
BDeviceList::BDeviceList(const char *aName) : BListPri( aName) {
//  bochs
  dprint("construct device list\n");
}

BDeviceList::~BDeviceList() {
}

BDevice *BDeviceList::FindDevice(const char *aName) {
  dprintf("Find device %s\n", aName);
  return ENull;
}

void BDeviceList::AddDevice(BDevice& aDevice) {
  dprintf("Add Device %x\n", aDevice);
}

