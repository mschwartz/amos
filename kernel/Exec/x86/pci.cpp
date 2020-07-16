#include <Exec/x86/pci.h>

// https : //github.com/nickwanninger/chariot/blob/cc3649af22126d21e3ff77296f73ec73157b1b8c/kernel/dev/pci.cpp

const TUint16 ADDRESS_PORT = 0xcf8;
const TUint16 DATA_PORT = 0xcfc;

const TUint8 OFFSET_DEVICE_ID = 0x00;     // upper 16 bits
const TUint8 OFFSET_VENDOR_ID = 0x00;     // lower 16 bits
const TUint8 OFFSET_STATUS = 0x04;        // upper 16 bits
const TUint8 OFFSET_COMMAND = 0x04;       // lower 16 bits
const TUint8 OFFSET_CLASS_CODE = 0x08;    // bits 24-31
const TUint8 OFFSET_SUBCLASS_CODE = 0x08; // bits 16-23
const TUint8 OFFSET_PROGIF = 0x08;        // bits 8-15;
const TUint8 OFFSET_HEADER_TYPE = 0x0c;   // header type bits 16-23
const TUint8 OFFSET_SECONDARY = 0x18;     // secondary bus bits
const TUint8 OFFSET_IRQ = 0x3c;           // lower 8 bits

const TUint16 MULTIFUNCTION_DEVICE = 0x80;
const TUint16 INVALID_VENDOR_ID = 0xffff;

const TUint16 SUBCLASS_PCI_TO_PCI_BRIDGE = 0x04;
const TUint16 CLASS_CODE_BRIDGE_DEVICE = 0x06;

class PCI_IO {
public:
  PCI_IO() {}
  ~PCI_IO() {
    bochs;
  }

public:
  TUint32 GetAddress(TUint32 aBus, TUint32 aSlot, TUint32 aFunc) {
    return (aBus << 16) | (aSlot << 11) | (aFunc << 8) | 0x80000000;
  }

  TUint32 ReadField(TUint32 aAddress, TUint8 aOffset) {
    outl(ADDRESS_PORT, aAddress | (aOffset & 0xfc));
    TUint32 ret = inl(DATA_PORT);
    // dlog("      ReadField(%x, %02x) = %08x\n", aAddress, aOffset, ret);
    return ret;
  }

  void WriteField(TUint32 aAddress, TUint8 aOffset, TUint32 aValue) {
    outl(ADDRESS_PORT, aAddress | (aOffset & 0xfc));
    return outl(DATA_PORT, aValue);
  }

  /******************************************************************************/

  TUint16 GetDeviceId(TUint32 aAddress) {
    // upper 16 bits
    TUint16 device_id = (ReadField(aAddress, OFFSET_DEVICE_ID) >> 16) & 0xffff;
    // dlog("      device id: %x\n:, device_id\n");
    return device_id;
  }

  TUint16 GetVendorId(TUint32 aAddress) {
    // lower 16 bits
    TUint16 vendor_id = ReadField(aAddress, OFFSET_VENDOR_ID) & 0xffff;
    // dlog("      vendor id: %x\n", vendor_id);
    return vendor_id;
  }

  TUint16 GetStatus(TUint32 aAddress) {
    // upper 16 bits
    return (ReadField(aAddress, OFFSET_STATUS) >> 16) & 0xffff;
  }
  TUint16 GetCommand(TUint32 aAddress) {
    // lower 16 bits
    return ReadField(aAddress, OFFSET_STATUS) & 0xffff;
  }

  TUint16 GetClassCode(TUint32 aAddress) {
    // bits 24-31
    return (ReadField(aAddress, OFFSET_CLASS_CODE) >> 24) & 0xff;
  }
  TUint16 GetSubclassCode(TUint32 aAddress) {
    // bits 16-23
    return (ReadField(aAddress, OFFSET_SUBCLASS_CODE) >> 16) & 0xff;
  }

  TUint16 GetRevisionId(TUint32 aAddress) {
    return (ReadField(aAddress, OFFSET_SUBCLASS_CODE) >> 16) & 0xff;
    
  }
  TUint16 GetProgIf(TUint32 aAddress) {
    // bits 8-15
    return (ReadField(aAddress, OFFSET_SUBCLASS_CODE) >> 8) & 0xff;
  }

  TUint16 GetHeaderType(TUint32 aAddress) {
    return (ReadField(aAddress, OFFSET_HEADER_TYPE) >> 16) & 0xff;
  }

  TUint16 GetSecondary(TUint32 aAddress) {
    return (ReadField(aAddress, OFFSET_SECONDARY) >> 8) & 0xff;
  }

  TUint16 GetIRQ(TUint32 aAddress) {
    return ReadField(aAddress, OFFSET_IRQ) & 0xff;
  }
};

static PCI_IO sio;

static const char *class_name(TInt16 aClass) {
  static const char *class_names[] = {
    [0x00] = "Unclassified",
    [0x01] = "Mass Storage Controller",
    [0x02] = "Network Controller",
    [0x03] = "Display Controller",
    [0x04] = "Multimedia Controller",
    [0x05] = "Memory Controller",
    [0x06] = "Bridge Device",
    [0x07] = "Simple Communication Controller",
    [0x08] = "Base System Peripheral",
    [0x09] = "Input Device Controller",
    [0x0a] = "Docking Station",
    [0x0b] = "Processor",
    [0x0c] = "Serial Bus Controller",
    [0x0d] = "Wireless Controller",
    [0x0e] = "Intelligent Controller",
    [0x0f] = "Satellite Communication Controller",
    [0x10] = "Encryption Controller",
    [0x11] = "Signal Processing Controller",
    [0x12] = "Processing Accelerator",
    [0x13] = "Non-Essential Instrumentation",
    [0x14] = "(Reserved)",
  };

  return (aClass > 0x14) ? "Unknown" : class_names[aClass];
}

void PCI::CheckFunction(TUint8 aBus, TUint8 aDevice, TUint8 aFunction) {
  TUint32 addr = sio.GetAddress(aBus, aDevice, aFunction);
  TUint16 vendorId = sio.GetVendorId(addr),
          deviceId = sio.GetDeviceId(addr),
          baseClass = sio.GetClassCode(addr),
          subClass = sio.GetSubclassCode(addr);

  if (baseClass == 0xff) {
    return;
  }

  PCIDevice *dev = new PCIDevice(aBus, aDevice, aFunction);
  dev->mBus = aBus;
  dev->mDevice = aDevice;
  dev->mFunction = aFunction;
  // dev->mVendorId = vendorId;
  // dev->mDeviceId = deviceId;
  // dev->mClassId = baseClass;
  // dev->mSubclassId = subClass;
  mDeviceList.AddTail(*dev);

  dlog("      vendorId(%x), baseClass %x(%s) subClass(%x) addr(%x) BRIDGE(%x) SUBBRIDGE(%x)\n",
    vendorId, baseClass, class_name(baseClass), subClass, addr, CLASS_CODE_BRIDGE_DEVICE, SUBCLASS_PCI_TO_PCI_BRIDGE);

  if ((baseClass == CLASS_CODE_BRIDGE_DEVICE) && (subClass == SUBCLASS_PCI_TO_PCI_BRIDGE)) {
    ScanBus(sio.GetSecondary(addr));
  }
}

void PCI::ScanBus(TUint8 aBusNumber) {
  TUint8 func = 0;

  for (TUint8 device = 0; device < 32; device++) {
    TUint32 addr = sio.GetAddress(aBusNumber, device, func);
    TUint16 type = sio.GetHeaderType(addr);
    if (type != 0xff) {
      dlog("    ScanBus(%d) addr(%x) type(%x)\n", aBusNumber, addr, type);
      if ((type & MULTIFUNCTION_DEVICE) != 0) {
        dlog("      MULTIFUNCION\n");
        for (func = 0; func < 7; func++) {
          CheckFunction(aBusNumber, device, func);
        }
      }
      else {
        CheckFunction(aBusNumber, device, 0);
      }
    }
  }
}

PCI::PCI() {
  dprint("\n");
  dlog("Construct PCI\n");

  TUint16 type = sio.GetHeaderType(sio.GetAddress(0, 0, 0));
  if ((type & MULTIFUNCTION_DEVICE) == 0) {
    dlog("  Single PCI host controller\n");
    ScanBus(0);
  }
  else {
    dlog("  Multiple PCI host controllers\n");
    for (TInt8 bus = 0; bus < 32; bus++) {
      ScanBus(bus);
    }
  }
  dprint("\n\n");
  dlog("============================================================================================================\n\n");
  dlog("Device List:\n");
  for (PCIDevice *d = (PCIDevice *)mDeviceList.First(); !mDeviceList.End(d); d = (PCIDevice *)d->mNext) {
    d->Dump();
  }
  dprint("\n");
  dlog("============================================================================================================\n");
  dprint("\n\n");
}

PCI::~PCI() {
}

typedef struct {
  TUint16 mPad0[27];
  TUint16 mModel[20];
  TUint16 mPad1[13];
  TUint32 mLbaSectors;
  TUint16 mPad2[21];
  TUint16 mLba48;
  TUint16 mPad3[16];
  TUint64 mLba48Sectors;
  TUint16 mPad4[152];
} PACKED TAtaIdentity;

PCIDevice::PCIDevice(TUint8 aBus, TUint8 aDevice, TUint8 aFunction) : BNode("temp") {
  TUint32 addr = sio.GetAddress(aBus, aDevice, aFunction);
  mAddress = addr;

  mBus = aBus;
  mDevice = aDevice;
  mFunction = aFunction;

  mVendorId = sio.GetVendorId(addr);
  mDeviceId = sio.GetDeviceId(addr);
  mCommand = sio.GetCommand(addr);
  mStatus = sio.GetStatus(addr);
  mRevisionId = sio.GetRevisionId(addr);
  mProgIf = sio.GetProgIf(addr);

  mSubclass = sio.GetSubclassCode(addr);
  mClass = sio.GetClassCode(addr);
  mHeaderType = sio.GetHeaderType(addr);

  mBar0 = sio.ReadField(addr, 0x10);
  mBar1 = sio.ReadField(addr, 0x14);
  mBar2 = sio.ReadField(addr, 0x18);
  mBar3 = sio.ReadField(addr, 0x1c);
  mBar4 = sio.ReadField(addr, 0x20);
  mBar5 = sio.ReadField(addr, 0x24);

  mSecondaryBus = sio.GetSecondary(addr);
  mInterruptLine = sio.GetIRQ(addr);

  SetName(class_name(mClass));
};
