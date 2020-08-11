#include "ioapic.hpp"

// https://ethv.net/workshops/osdev/notes-notes-3

IoApic::IoApic(TUint64 aAddress) {
  mAddress = aAddress;
  MapIRQ(2, 0, 0);
}

void IoApic::MapIRQ(TUint8 aIrq, TUint64 aApicId, TUint8 aVector) {
  const TUint32 low_index = 0x10 + aIrq * 2;
  const TUint32 high_index = 0x10 + aIrq * 2 + 1;

  TUint32 high = ReadRegister(high_index);
  high &= ~0xff000000;
  high |= aApicId << 24;
  WriteRegister(high_index, high);

  TUint32 low = ReadRegister(low_index);
  low &= ~(1 << 16); // unmask IRQ
  low &= ~(1 << 11); // set to physical delivery mode
  low &= ~0x700;     // set to fixed delivery mode
  // set delivery vector
  low &= ~0xff;
  low |= aVector;

  WriteRegister(low_index, low);
}
