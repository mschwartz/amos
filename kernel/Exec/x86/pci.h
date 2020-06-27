#ifndef EXEC_X86_PCI_H
#define EXEC_X86_PCI_H

#include <Exec/BBase.h>

class PCI_IO;

class PCI : public BBase {
public:
  PCI();
  ~PCI();
protected:
  void ScanBus(TUint8 aBusNumber);
  void Scan();
protected:
  PCI_IO *mIO;
  
};

#if 0
#define PCI_VENDOR_ID 0x00
#define PCI_DEVICE_ID 0x02
#define PCI_COMMAND 0x04
#define PCI_STATUS 0x06		  // 2
#define PCI_REVISION_ID 0x08  // 1

#define PCI_PROG_IF 0x09		  // 1
#define PCI_SUBCLASS 0x0a		  // 1
#define PCI_CLASS 0x0b			  // 1
#define PCI_CACHE_LINE_SIZE 0x0c  // 1
#define PCI_LATENCY_TIMER 0x0d	  // 1
#define PCI_HEADER_TYPE 0x0e	  // 1
#define PCI_BIST 0x0f			  // 1
#define PCI_BAR0 0x10			  // 4
#define PCI_BAR1 0x14			  // 4
#define PCI_BAR2 0x18			  // 4
#define PCI_BAR3 0x1C			  // 4
#define PCI_BAR4 0x20			  // 4
#define PCI_BAR5 0x24			  // 4

#define PCI_INTERRUPT_LINE 0x3C	 // 1

#define PCI_SECONDARY_BUS 0x19	// 1

#define PCI_TYPE_BRIDGE 0x0604
#define PCI_TYPE_SATA 0x0106

#define PCI_ADDRESS_PORT 0xCF8
#define PCI_VALUE_PORT 0xCFC

#define PCI_INVALID_VENDOR_ID 0xFFFF
#define PCI_MULTIFUNCTION_DEVICE 0x80

#define PCI_CLASS_CODE_MASS_STORAGE 0x01
#define PCI_CLASS_CODE_BRIDGE_DEVICE 0x06

#define PCI_SUBCLASS_IDE 0x01
#define PCI_SUBCLASS_PCI_TO_PCI_BRIDGE 0x04

#define PCI_COMMAND_REG_BUS_MASTER (1 << 2)

struct pci_device {
	int32_t address;
	int32_t deviceID, vendorID;
	uint32_t bar0, bar1, bar2, bar3, bar4, bar5, bar6;
	struct list_head sibling;
};

#endif

#endif
