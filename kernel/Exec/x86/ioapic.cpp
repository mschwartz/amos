#include "ioapic.hpp"

// https://ethv.net/workshops/osdev/notes-notes-3

IoApic::IoApic(TUint64 aAddress) {
  mAddress = aAddress;
}
