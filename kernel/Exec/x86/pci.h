#ifndef EXEC_X86_PCI_H
#define EXEC_X86_PCI_H

// https : //github.com/nickwanninger/chariot/blob/cc3649af22126d21e3ff77296f73ec73157b1b8c/kernel/dev/pci.cpp

#include <Exec/BBase.h>
#include <Types/BList.h>

const TUint8 PCI_CLASS_UNCLASSIFIED = 0x00;
const TUint8 PCI_CLASS_MASS_STORAGE_CONTROLLER = 0x01;
const TUint8 PCI_CLASS_NETWORK_CONTROLLER = 0x02;
const TUint8 PCI_CLASS_DISPLAY_CONTROLLER = 0x03;
const TUint8 PCI_CLASS_MULTIMEDIA_CONTROLLER = 0x04;
const TUint8 PCI_CLASS_MEMORY_CONTROLLER = 0x05;
const TUint8 PCI_CLASS_BRIDGE_DEVICE = 0x06;
const TUint8 PCI_CLASS_SIMPLE_COMMUNICATION_CONTROLLER = 0x07;
const TUint8 PCI_CLASS_BASE_SYATEM_PERIPHERAL = 0x08;
const TUint8 PCI_CLASS_INPUT_DEVICE_CONTROLLER = 0x09;
const TUint8 PCI_CLASS_DOCKING_STATION = 0x0a;
const TUint8 PCI_CLASS_PROCESSOR = 0x0b;
const TUint8 PCI_CLASS_SERIAL_BUS_CONTROLLER = 0x0c;
const TUint8 PCI_CLASS_WIRELESS_CONTROLLER = 0x0d;
const TUint8 PCI_CLASS_INTELLIGENT_CONTROLLER = 0x0e;
const TUint8 PCI_CLASS_SATELLITE_COMMUNICATION_CONTROLLER = 0x0f;
const TUint8 PCI_CLASS_ENCRYPTION_CONTROLLER = 0x10;
const TUint8 PCI_CLASS_SIGNAL_PROCESSING_CONTROLLER = 0x11;
const TUint8 PCI_CLASS_PROCESSING_ACCELERATOR = 0x12;
const TUint8 PCI_CLASS_NON_ESSENTIAL_INSTRUMENTATION = 0x13;
const TUint8 PCI_CLASS_RESERVED = 0x14;

struct TPciDevice : public BNode {
  TUint32 mAddress = 0;
  TUint8 mBus = 0;
  TUint8 mDevice = 0;
  TUint8 mFunction = 0;
  //
  TUint16 mVendorId = 0;
  TUint16 mDeviceId = 0;
  TUint16 mCommand = 0;
  TUint16 mStatus = 0;
  TUint8 mRevisionId = 0;
  TUint8 mProgIf = 0;
  TUint8 mSubclass = 0;
  TUint8 mClass = 0;
  TUint8 mCacheLineSize = 0;
  TUint8 mLatencyTimer = 0;
  TUint8 mHeaderType = 0;
  TUint8 mBist = 0;
  TUint16 mBar0 = 0;
  TUint16 mBar1 = 0;
  TUint16 mBar2 = 0;
  TUint16 mBar3 = 0;
  TUint16 mBar4 = 0;
  TUint16 mBar5 = 0;
  TUint32 mCardbusCisPointer = 0;
  TUint16 mSubsystemId = 0;
  TUint16 mSubsystemVendorId = 0;
  TUint32 mExpansionRomBaseAddress = 0;
  TUint8 mCapabilitiesPointer = 0;
  TUint8 mSecondaryBus = 0;
  TUint8 mMaxLatency = 0;
  TUint8 mMinGrant = 0;
  TUint8 mInterruptPin = 0;
  TUint8 mInterruptLine = 0;

public:
  TPciDevice(TUint8 aBus, TUint8 aDevice, TUint8 aFunction);

public:
  TBool IsAtaDevice() { return mClass == 1; }

public:
  void Dump() {
    dprint("\n\n");
    dlog("PCIDevice(%s)\n", mNodeName);
    dlog("          mAddress: 0x%x mBus(%d) mDevice(%d) mFunction(%d)\n", mAddress, mBus, mDevice, mFunction);
    dlog("         mVendorId: 0x%x\n", mVendorId);
    dlog("         mDeviceId: 0x%x\n", mDeviceId);

    dlog("          mCommand: 0x%x\n", mCommand);
    dlog("           mStatus: 0x%x\n", mStatus);

    dlog("            mClass: 0x%02x\n", mClass);
    dlog("         mSubclass: 0x%02x\n", mSubclass);

    dlog("     mSecondaryBus: %d\n", mSecondaryBus);
    dlog("    mInterruptLine: %d\n", mInterruptLine);
    dlog("               BAR: BAR0(0x%x) BAR1(0x%x) BAR2(0x%x) BAR3(0x%x) BAR4(0x%x) BAR5(0x%x)\n",
      mBar0, mBar1, mBar2, mBar3, mBar4, mBar5);
  }
};

class PCI : public BBase {
public:
  PCI();
  ~PCI();

protected:
  void CheckFunction(TUint8 aBus, TUint8 aDevice, TUint8 aFunction);
  void ScanBus(TUint8 aBusNumber);

public:
  TPciDevice *FirstDevice() { return (TPciDevice *)mDeviceList.First(); }
  TBool EndDevices(TPciDevice *aDevice) { return mDeviceList.End(aDevice); }
  TPciDevice *NextDevice(TPciDevice *aDevice) { return (TPciDevice *)mDeviceList.Next(aDevice); }

protected:
  BList mDeviceList;
};

#endif
