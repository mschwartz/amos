#ifndef AMOS_KERNEL_EXEC_X86_IOAPIC_HPP
#define AMOS_KERNEL_EXEC_X86_IOAPIC_HPP

// https://ethv.net/workshops/osdev/notes-notes-3

#include <Types.hpp>

const TInt MAX_IOAPIC = 4;

class IoApic {
public:
  IoApic(TUint64 aAddress);

public:
  TUint64 Address() { return mAddress; }

public:
  void WriteRegister(TUint8 aOffset, TUint32 aValue) {
    *(volatile TUint32 *)(mAddress) = aOffset;       // IOREGSEL
    *(volatile TUint32 *)(mAddress + 0x10) = aValue; // IOWIN
  }
  TUint32 ReadRegister(TUint8 aOffset) {
    *(volatile TUint32 *)(mAddress) = aOffset;     // IOREGSEL
    return *(volatile TUint32 *)(mAddress + 0x10); // IOWIN
  }

public:
  void MapIRQ(TUint8 aIrq, TUint64 aApicId, TUint8 aVector);

protected:
  TUint64 mAddress;
};

#endif
