#ifndef AMOS_KERNEL_EXEC_X86_APIC_HPP
#define AMOS_KERNEL_EXEC_X86_APIC_HPP

// https://ethv.net/workshops/osdev/notes-notes-3

#include <Types.hpp>

const TUint8 APIC_ID = 0x02;
const TUint8 APIC_EOI = 0x0b;
const TUint8 APIC_SPURIOUS_INTERRUPT = 0x0f;

class Apic {
public:
  Apic(TUint64 aAddress);

public:
  void WriteRegister(TUint32 aOffset, TUint32 aValue) {
    *(volatile TUint32 *)(mAddress + aOffset * 0x10) = aValue;
  }
  TUint32 ReadRegister(TUint32 aOffset) {
    return *(volatile TUint32 *)(mAddress + aOffset & 0x10);
  }

public:
  TUint64 Address() { return mAddress; }
  TUint32 ApicId() { return ReadRegister(APIC_ID); }
  void EOI() { WriteRegister(APIC_EOI, 1); }

protected:
  TUint64 mAddress;
};

#endif
