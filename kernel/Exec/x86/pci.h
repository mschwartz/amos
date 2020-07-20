#ifndef EXEC_X86_PCI_H
#define EXEC_X86_PCI_H

// https : //github.com/nickwanninger/chariot/blob/cc3649af22126d21e3ff77296f73ec73157b1b8c/kernel/dev/pci.cpp

#include <Exec/BBase.h>
#include <Types/BList.h>

enum EPciClass {
  EPciUnclassified,                     // [0x00] = "Unclassified",
  EpciMassStorageController,            //   [0x01] = "Mass Storage Controller",
  EPciNetworkController,                //   [0x02] = "Network Controller",
  EPciDisplayController,                //   [0x03] = "Display Controller",
  EPciMultimediaController,             //   [0x04] = "Multimedia Controller",
  EPciMemoryController,                 // [0x05] = "Memory Controller",
  EPciBridgeDevice,                     // [0x06] = "Bridge Device",
  EPciSimpleCommunicationController,    // [0x07] = "Simple Communication Controller",
  EPciBaseSystemPeripheral,             // [0x08] = "Base System Peripheral",
  EPciInputDeviceContoller,             //   [0x09] = "Input Device Controller",
  EPciDockingStation,                   //   [0x0a] = "Docking Station",
  EPciProcessor,                        //   [0x0b] = "Processor",
  EPciSerialBusController,              //   [0x0c] = "Serial Bus Controller",
  EPciWirelessController,               //   [0x0d] = "Wireless Controller",
  EPciIntellientController,             //   [0x0e] = "Intelligent Controller",
  EPciSatelliteCommunicationController, //   [0x0f] = "Satellite Communication Controller",
  EPciEncryptionController,             //   [0x10] = "Encryption Controller",
  EPciSignalProcessingController,       //   [0x11] = "Signal Processing Controller",
  EPciProcessingAccelerator,            //   [0x12] = "Processing Accelerator",
  EPciNonEssentialInstrumentation,      //   [0x13] = "Non-Essential Instrumentation",
  EPciReserved,                         //   [0x14] = "(Reserved)",
  EPciMax = EPciReserved,
};

struct TPciDevice : public BNode {
  TUint16 mBus, mDevice, mFunction;
  TUint32 mPortBase;
  TUint16 mVendorId, mDeviceId;
  TUint32 mBar0, mBar1, mBar2, mBar3, mBar4, mBar5;
  TUint8 mClassId, mSubclassId, mInterfaceId, mRevision, mIRQ;

public:
  TPciDevice(TUint8 aBus, TUint8 aDevice, TUint8 aFunction);

public:
  void Dump(TBool aVerbose = ETrue) {
    if (aVerbose) {
      dprint("\n\n");
      dlog("TPciDevice(%s)\n", mNodeName);
      dlog("     mVendorId: %x\n", mVendorId);
      dlog("     mDeviceId: %x\n", mDeviceId);
      dlog("      mClassId: %x\n", mClassId);
      dlog("   mSubclassId: %x\n", mSubclassId);
      dlog("         mBars: 0(0x%04x) 1(0x%04x)  2(0x%04x) 3(0x%04x) 4(0x%04x) 5(0x%04x)\n", mBar0, mBar1, mBar2, mBar3, mBar4, mBar5);
    }
    else {
      dlog("  TPciDevice(%s) mVendorId(0x%04x) mDeviceId(%0x04x) mClassId(0x%04x) mSubClassid(0x%04x)\n",
        mNodeName, mVendorId, mDeviceId, mClassId, mSubclassId);
      dlog("      mBars: 0(0x%04x) 1(0x%04x)  2(0x%04x) 3(0x%04x) 4(0x%04x) 5(0x%04x)\n", mBar0, mBar1, mBar2, mBar3, mBar4, mBar5);
    }
  }
};

class PCI : public BBase {
public:
  PCI();
  ~PCI();

public:
  static const char *ClassName(TUint8 aClass);

protected:
  void CheckFunction(TUint8 aBus, TUint8 aDevice, TUint8 aFunction);
  void ScanBus(TUint8 aBusNumber);

public:
  TPciDevice *First() { return (TPciDevice *)mDeviceList.First(); }
  TPciDevice *Next(TPciDevice *aDevice) { return (TPciDevice *)mDeviceList.Next((BNode *)aDevice); }
  TBool End(TPciDevice *aDevice) { return (TPciDevice *)mDeviceList.End((BNode *)aDevice); }

protected:
  BList mDeviceList;
};

#endif
