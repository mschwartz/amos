#include <Exec/x86/apic.hpp>

// https://ethv.net/workshops/osdev/notes-notes-3

Apic::Apic(TUint64 aAddress) {
  dlog("COnstruct aPic\n");
  mAddress = aAddress;
  TUint32 val = ReadRegister(APIC_SPURIOUS_INTERRUPT);
  val |= (1 << 8);
  WriteRegister(APIC_SPURIOUS_INTERRUPT, val);
}
