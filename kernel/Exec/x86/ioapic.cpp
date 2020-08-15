#include "ioapic.hpp"
#include <Exec/BInterrupt.hpp>

// https://ethv.net/workshops/osdev/notes-notes-3

const TUint64 CPU0 = 0;

IoApic::IoApic(TUint64 aIndex, TUint64 aId, TUint64 aAddress, TUint64 aBase) {
  mIndex = aIndex;
  mId = aId;
  mAddress = aAddress;
  mBase = aBase;
  dlog("Read ID %d arg %d\n", ReadRegister(IOAPIC_ID) >> 24, aId);
  mAddress += mBase;
  TUint64 ioapicVersion = ReadRegister(IOAPIC_VERSION);
  mVersion = ioapicVersion & 0x0ff;
  mMaxRedirectionEntries = (ioapicVersion >> 16) & 0xff;
  Dump();
  for (TInt i=0; i<256; i++) {
    mApicIds[i] = mVectors[i] = 0;
  }

  // remap
  for (TInt i = 0; i < mMaxRedirectionEntries; i++) {
    MapIRQ(i, CPU0, i + 32);
  }
  // 0 is APIC timer IRQ, 2 is PIC timer IRQ
  // MapIRQ(2, CPU0, IRQ_TIMER);
}

void IoApic::EnableIRQ(TUint16 aIrq) {
  MapIRQ(aIrq, mApicIds[aIrq], mVectors[aIrq], ETrue);
}

void IoApic::DisableIRQ(TUint16 aIrq) {
  MapIRQ(aIrq, mApicIds[aIrq], mVectors[aIrq], EFalse);
}

void IoApic::MapIRQ(TUint8 aIrq, TUint64 aApicId, TUint8 aVector, TBool aEnable) {
  dlog("MapIRQ aIrq(%d) aApicId(%d) aVector(%d) Enable(%d)\n", aIrq, aApicId, aVector, aEnable);
  // aEnable = EFalse;
  mApicIds[aIrq] = aApicId;
  mVectors[aIrq] = aVector;

  const TUint32 low_index = 0x10 + aIrq * 2;
  const TUint32 high_index = 0x10 + aIrq * 2 + 1;

  TIoApicRedirectionEntry entry = {
    .mVector = aVector,
    .mDeliveryMode = IOAPIC_DELIVERY_MODE_DEFAULT,
    .mDestinationMode = IOAPIC_DESTINATION_MODE_DEFAULT,
    .mPinPolarity = IOAPIC_PIN_POLARITY_DEFAULT,
    .mTriggerMode = IOAPIC_TRIGGER_MODE_DEFAULT,
    .mMask = aEnable ? IOAPIC_MASK_ENABLE_IRQ : IOAPIC_MASK_DISABLE_IRQ,
    .mReserved = 0,
    .mDestintation = aApicId,
  };

  struct redirection_entry {
    TUint32 low;
    TUint32 high;
  } *ptr = (redirection_entry *)&entry;

  WriteRegister(high_index, ptr->high);
  WriteRegister(low_index, ptr->low);
}
