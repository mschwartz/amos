#ifndef AMOS_KERNEL_EXEC_X86_IOAPIC_HPP
#define AMOS_KERNEL_EXEC_X86_IOAPIC_HPP

// https://ethv.net/workshops/osdev/notes-notes-3

#include <Types.hpp>

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

protected:
  TUint64 mAddress;
};

#endif
