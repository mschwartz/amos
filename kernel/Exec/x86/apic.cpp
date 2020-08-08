#include <Exec/x86/apic.hpp>

// https://ethv.net/workshops/osdev/notes-notes-3

Apic::Apic(TUint64 aAddress) {
  dlog("COnstruct aPic\n");
  mAddress = aAddress;
  TUint32 val = ReadRegister(APIC_SPURIOUS_INTERRUPT);
  val |= (1 << 8);
  WriteRegister(APIC_SPURIOUS_INTERRUPT, val);
}

void Apic::SendIPI(TUint8 aApicId, TUint8 aVector) {
  while (ReadRegister(0x30) & (1 << 12)) {
    
  }
  TUint32 control = aVector;
  control |= 0x4500;
  WriteRegister(0x30 * 0x10, control);
}

void Apic::SendSIPI(TUint8 aApicId, TUint8 aVector) {
  while (ReadRegister(0x30) & (1 << 12)) {
    
  }
  TUint32 control = aVector;
  control |= 0x4600;
  WriteRegister(0x30 * 0x10, control);
}
