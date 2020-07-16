#include <Exec/x86/pci.h>

// https : //github.com/nickwanninger/chariot/blob/cc3649af22126d21e3ff77296f73ec73157b1b8c/kernel/dev/pci.cpp

const TUint16 PCI_ADDRESS_PORT = 0xcf8;
const TUint16 PCI_VALUE_PORT = 0xcfc;

const TUint16 PCI_NONE = 0xffff;

const TUint8 PCI_VENDOR_ID = 0x00;       // 2
const TUint8 PCI_DEVICE_ID = 0x02;       // 2
const TUint8 PCI_COMMAND = 0x04;         // 2
const TUint8 PCI_STATUS = 0x06;          // 2
const TUint8 PCI_REVISION_ID = 0x08;     // 1
const TUint8 PCI_PROG_IF = 0x09;         // 1
const TUint8 PCI_SUBCLASS = 0x0a;        // 1
const TUint8 PCI_CLASS = 0x0a;           // 1
const TUint8 PCI_CACHE_LINE_SIZE = 0x0c; // 1
const TUint8 PCI_LATENCY_TIMER = 0x0d;   // 1
const TUint8 PCI_HEADER_TYPE = 0x0e;     // 1
const TUint8 PCI_BIST = 0x0f;            // 1
const TUint8 PCI_BAR0 = 0x10;            // 4
const TUint8 PCI_BAR1 = 0x14;            // 4
const TUint8 PCI_BAR2 = 0x18;            // 4
const TUint8 PCI_BAR3 = 0x1c;            // 4
const TUint8 PCI_BAR4 = 0x20;            // 4
const TUint8 PCI_BAR5 = 0x24;            // 4
const TUint8 PCI_INTERRUPT_LINE = 0x38;  // 1
const TUint8 PCI_SECONDARY_BUS = 0x38;   // 1

const TUint8 PCI_HEADER_TYPE_DEVICE = 0;
const TUint8 PCI_HEADER_TYPE_BRIDGE = 1;
const TUint8 PCI_HEADER_TYPE_CARDBUS = 2;

const TInt16 PCI_TYPE_BRIDGE = 0x0004;
const TInt16 PCI_TYPE_SATA = 0x0100;

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

static inline TUint8 pci_extract_bus(TUint32 aDevice) {
  return (aDevice >> 16) & 0xff;
}

static inline TUint8 pci_extract_slot(TUint32 device) {
  return (device >> 11) & 0xff;
}

static inline TUint8 pci_extract_func(TUint32 device) {
  return (device >> 8) & 0xff;
}

static inline TUint32 pci_get_addr(TUint8 aBus, TUint8 aDevice, TUint8 aFunction, TInt aField) {
  return 0x80000000 |
         (aBus << 16) |
         (aDevice << 11) |
         (aFunction << 8) |
         ((aField)&0xFC);
}

static inline TUint32 pci_box_device(TInt aBus, TInt aSlot, TInt aFunc) {
  return (aBus << 16) | (aSlot << 11) | (aFunc << 8) | 0x80000000;
  // return (TUint32)((bus << 16) | (slot << 8) | func);
}

#if 0
static void pci_write_field(TUint32 device, TInt field, TInt size, TUint32 value) {
  outl(PCI_ADDRESS_PORT, pci_get_addr(device, field));
  outl(PCI_VALUE_PORT, value);
}
#endif

static TUint32 pci_read_field(TUint8 aBus, TUint8 aDevice, TUint8 aFunction, TUint32 field) {
  TUint64 a = pci_get_addr(aBus, aDevice, aFunction, field);
  outl(PCI_ADDRESS_PORT, a);
  return inl(PCI_VALUE_PORT);
}

const TUint16 MULTIFUNCTION_DEVICE = 0x80;
const TUint16 SUBCLASS_PCI_TO_PCI_BRIDGE = 0x04;
const TUint16 CLASS_CODE_BRIDGE_DEVICE = 0x06;

PCIDevice::PCIDevice(TUint8 aBus, TUint8 aDevice, TUint8 aFunction) : BNode("temp") {
  TInt32 w = pci_read_field(aBus, aDevice, aFunction, 8);
  mClass = (w >> 24) & 0xff;
  if (mClass == 0xff) {
    return;
  }

  w = pci_read_field(aBus, aDevice, aFunction, 0);
  mDeviceId = (w >> 16) & 0xffff;
  mVendorId = w & 0xffff;

  w = pci_read_field(aBus, aDevice, aFunction, 4);
  mStatus = (w >> 16) & 0xffff;
  mCommand = w & 0xffff;

  this->SetName(class_name(mClass));
  mSubclass = (w >> 16) & 0xff;
  mProgIf = (w >> 8) & 0xff;
  mRevisionId = w & 0xff;

  w = pci_read_field(aBus, aDevice, aFunction, 0x0c);
  mBist = (w >> 24) & 0xff;
  mHeaderType = (w >> 16) & 0xff;
  mLatencyTimer = (w >> 8) & 0xff;
  mCacheLineSize = w & 0xff;

  mBar0 = pci_read_field(aBus, aDevice, aFunction, 0x10);
  mBar1 = pci_read_field(aBus, aDevice, aFunction, 0x14);
  mBar2 = pci_read_field(aBus, aDevice, aFunction, 0x18);
  mBar3 = pci_read_field(aBus, aDevice, aFunction, 0x1c);
  mBar4 = pci_read_field(aBus, aDevice, aFunction, 0x20);
  mBar5 = pci_read_field(aBus, aDevice, aFunction, 0x24);

  mCardbusCisPointer = pci_read_field(aBus, aDevice, aFunction, 0x28);

  w = pci_read_field(aBus, aDevice, aFunction, 0x2c);
  mSubsystemId = (w >> 16) & 0xff;
  mSubsystemVendorId = w & 0xff;

  mExpansionRomBaseAddress = pci_read_field(aBus, aDevice, aFunction, 0x30);

  w = pci_read_field(aBus, aDevice, aFunction, 0x34);
  mCapabilitiesPointer = w & 0xff;

  w = pci_read_field(aBus, aDevice, aFunction, 0x38);
  mSecondaryBus = (w >> 8) & 0xff;

  w = pci_read_field(aBus, aDevice, aFunction, 0x3c);
  mMaxLatency = (w >> 24) & 0xff;
  mMinGrant = (w >> 16) & 0xff;
  mInterruptPin = (w >> 8) & 0xff;
  mInterruptLine = w & 0xff;
};

#if 0
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

  TUint32 ReadField(TUint32 aAddress, TUint8 aOffset, TInt aSize) {
    outl(PCI_ADDRESS_PORT, aAddress | (aOffset & 0xfc));
    switch (aSize) {
      case 4:
        return (TUint32)inl(PCI_VALUE_PORT);
        break;
      case 2:
        return (TUint32)inw(PCI_VALUE_PORT);
        break;
      case 1:
        return (TUint32)inb(PCI_VALUE_PORT);
        break;
      default:
        dlog("ReadField(%d) size invalid\n", aSize);
        bochs;
        return 0;
    }
  }

  void WriteField(TUint32 aAddress, TUint8 aOffset, TUint32 aValue) {
    outl(PCI_ADDRESS_PORT, aAddress | (aOffset & 0xfc));
    return outl(PCI_VALUE_PORT, aValue);
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
#endif

// static PCI_IO sio;

static TUint8 get_base_class(TUint32 aBus, TUint32 aDevice, TUint32 aFunction) {
  TUint16 w = pci_read_field(aBus, aDevice, aFunction, 0x08);
  return (w >> 24) & 0xff;
}

static TUint8 get_subclass(TUint8 aBus, TUint32 aDevice, TUint32 aFunction) {
  TUint16 w = pci_read_field(aBus, aDevice, aFunction, 0x08);
  return (w >> 16) & 0xff;
}

static TUint8 get_secondary_bus(TUint8 aBus, TUint8 aDevice, TUint8 aFunction) {
  TUint16 w = pci_read_field(aBus, aDevice, aFunction, 0x38);
  return (w >> 8) & 0xff;
}

static TUint16 get_vendor_id(TUint8 aBus, TUint8 aDevice, TUint8 aFunction) {
  TUint32 w = pci_read_field(aBus, aDevice, aFunction, 0x38);
  return w & 0xffff;
}

static TUint16 get_header_type(TUint8 aBus, TUint8 aDevice, TUint8 aFunction) {
  TUint32 w = pci_read_field(aBus, aDevice, aFunction, 0x0c);
  return (w >> 16) & 0xff;
}

void PCI::CheckFunction(TUint8 aBus, TUint8 aDevice, TUint8 aFunction) {
  dlog("    check funciton(%d,%d<%d)\n", aBus, aDevice, aFunction);
  TUint8 baseClass = get_base_class(aBus, aDevice, aFunction);
  TUint8 subClass = get_subclass(aBus, aDevice, aFunction);

  PCIDevice *d = new PCIDevice(aBus, aDevice, aFunction);
  d->mBus = aBus;
  d->mDevice = aDevice;
  d->mFunction = aFunction;
  d->Dump();
  mDeviceList.AddTail(*d);
  d->Dump();
  
  if (baseClass == 0x06 && subClass == 0x04) {
    TUint8 secondaryBus = get_secondary_bus(aBus, aDevice, aFunction);
    CheckBus(secondaryBus);
  }
}

void PCI::CheckDevice(TUint8 aBus, TUint8 aDevice, TUint8 aFunction) {

  TUint8 func = 0;
  TUint16 vendorId = get_vendor_id(aBus, aDevice, func);
  dlog("  check device(%d, %d, %d) vendorId(%x)\n", aBus, aDevice, aFunction, vendorId);
  if (vendorId == 0xffff) {
    return; // no device
  }
  TUint16 headerType = get_header_type(aBus, aDevice, func);
  if ((headerType & 0x80) == 0) {
    // multifunciton device
    for (func = 1; func < 8; func++) {
      if (get_vendor_id(aBus, aDevice, func) != 0xffff) {
        CheckFunction(aBus, aDevice, func);
      }
    }
  }
}

void PCI::CheckBus(TUint8 aBus) {
  dlog("check bus %d\n", aBus);
  for (TUint8 device = 0; device < 32; device++) {
    CheckDevice(aBus, device, 0);
  }
}

void PCI::CheckAllBuses() {
  TUint8 function, bus;
  TUint8 headerType = get_header_type(0, 0, 0);
  if ((headerType & 0x80) == 0) {
    // single bus
    CheckBus(0);
  }
  else {
    for (function = 0; function < 8; function++) {
      if (get_vendor_id(0, 0, function) != 0xffff) {
	break;
      }
      bus = function;
      CheckBus(bus);
    }
  }
}

PCI::PCI() {
  dprint("\n");
  dlog("Construct PCI\n");

  CheckAllBuses();
  dlog("Device List:\n");
  for (PCIDevice *d = (PCIDevice *)mDeviceList.First(); !mDeviceList.End(d); d = (PCIDevice *)d->mNext) {
    d->Dump();
  }
  dprint("\n\n");
}

PCI::~PCI() {
}
