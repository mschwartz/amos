#include <Exec/x86/pci.h>

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
    dlog("      ReadField(%x, %02x) = %08x\n", aAddress, aOffset, ret);
    return ret;
  }

  void WriteField(TUint32 aAddress, TUint8 aOffset, TUint32 aValue) {
    outl(ADDRESS_PORT, aAddress | (aOffset & 0xfc));
    return outl(DATA_PORT, aValue);
  }

  /******************************************************************************/

  TUint16 GetDeviceId(TUint32 aAddress) {
    // upper 16 bits
    TUint16 device_id =  (ReadField(aAddress, OFFSET_DEVICE_ID) >> 16) & 0xffff;
    dlog("      device id: %x\n:, device_id\n");
    return device_id;
  }

  TUint16 GetVendorId(TUint32 aAddress) {
    // lower 16 bits
    TUint16 vendor_id = ReadField(aAddress, OFFSET_VENDOR_ID) & 0xffff;
    dlog("      vendor id: %x\n", vendor_id);
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

PCI::PCI() {
  mIO = new PCI_IO();
  dlog("CONSTRUCT PCI\n");
  Scan();
}

PCI::~PCI() {
  delete mIO;
}

void PCI::ScanBus(TUint8 aBusNumber) {
  TUint8 func = 0;
  for (TUint8 device = 0; device < 32; device++) {
    TUint32 addr = mIO->GetAddress(aBusNumber, device, func);
    TUint16 type = mIO->GetHeaderType(addr);
    dlog("    ScanBus(%d) addr(%x) type(%x)\n", aBusNumber, addr, type);
    if (type & MULTIFUNCTION_DEVICE) {
      for (func = 0; func < 7; func++) {
        // check function
      }
    }
    else {
      // check funciton
    }
  }
}

void PCI::Scan() {
  TUint16 type = mIO->GetHeaderType(mIO->GetAddress(0, 0, 0));
  dlog("  SCAN(%x)\n", type);
  if ((type & MULTIFUNCTION_DEVICE)) {
    bochs;
    for (TUint8 bus = 0; bus < 8; bus++) {
      if (mIO->GetVendorId(mIO->GetAddress(0, 0, bus)) == INVALID_VENDOR_ID) {
        ScanBus(bus);
      }
    }
  }
  else {
    ScanBus(0);
  }
}
