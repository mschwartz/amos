#ifndef AMOS_KERNEL_EXEC_X86_IOAPIC_HPP
#define AMOS_KERNEL_EXEC_X86_IOAPIC_HPP

// https://ethv.net/workshops/osdev/notes-notes-3

#include <Types.hpp>

// IOAPIC registers
const TUint32 IOAPIC_ID = 0x00;
const TUint32 IOAPIC_VERSION = 0x01;
const TUint32 IOAPIC_CARB = 0x02;

// IOAPIC REDIRECTION TABLE
const TUint32 IOAPIC_REDIRECTION_TABLE = 0x16;
const TUint32 IOAPIC_MAX_REDIRECTION_ENTRIES = 24;

typedef struct {
  TUint64 mVector : 8;
  TUint64 mDeliveryMode : 3;
  TUint64 mDestinationMode : 1;
  TUint64 mDeliveryStatus : 1;
  TUint64 mPinPolarity : 1;
  TUint64 mRemoteIRR : 1;
  TUint64 mTriggerMode : 1;
  TUint64 mMask : 1;
  TUint64 mReserved : 39;
  TUint64 mDestintation : 8;
} TIoApicRedirectionEntry;

const TUint32 IOAPIC_DELIVERY_MODE_FIXED = 0x00;
const TUint32 IOAPIC_DELIVERY_MODE_LOWEST_PRIORTY = 0x01;
const TUint32 IOAPIC_DELIVERY_MODE_SMI = 0x02;
const TUint32 IOAPIC_DELIVERY_MODE_NMI = 0x04;
const TUint32 IOAPIC_DELIVERY_MODE_INT = 0x05;
const TUint32 IOAPIC_DELIVERY_MODE_EXTINT = 0x07;
const TUint32 IOAPIC_DELIVERY_MODE_DEFAULT = IOAPIC_DELIVERY_MODE_FIXED;

// In physical destination mode, bits 59:56 contain an APIC ID.
const TUint32 IOAPIC_DESTINATION_MODE_PHYSICAL = 0x00;
// In logical destination mode, bits 59:56 specifies a set of processors.
const TUint32 IOAPIC_DESTINATION_MODE_LOGICAL = 0x01;
const TUint32 IOAPIC_DESTINATION_MODE_DEFAULT = IOAPIC_DESTINATION_MODE_PHYSICAL;

const TUint32 IOAPIC_PIN_POLARITY_ACTIVE_HIGH = 0x00;
const TUint32 IOAPIC_PIN_POLARITY_ACTIVE_LOW = 0x01;
const TUint32 IOAPIC_PIN_POLARITY_DEFAULT = IOAPIC_PIN_POLARITY_ACTIVE_HIGH;

const TUint32 IOAPIC_TRIGGER_MODE_EDGE = 0x00;
const TUint32 IOAPIC_TRIGGER_MODE_LEVEL = 0x01;
const TUint32 IOAPIC_TRIGGER_MODE_DEFAULT = IOAPIC_TRIGGER_MODE_EDGE;

const TUint32 IOAPIC_MASK_ENABLE_IRQ = 0x00;
const TUint32 IOAPIC_MASK_DISABLE_IRQ = 0x01;

class IoApic {
public:
  IoApic(TUint64 aIndex, TUint64 aId, TUint64 aAddress, TUint64 aBase);
  void Dump() {
    dlog("IoApic at %x mIndex(%d) mId(%d) mAddress(%x) mBase(%x) mVersion(%d) max(%d)\n", this, mIndex, mId, mAddress, mBase, mVersion, mMaxRedirectionEntries);
  }

public:
  TUint64 Address() { return mAddress; }
  TUint64 Base() { return mBase; }

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
  // map IRQ number to a vector number on the CPU with aApicID
  void MapIRQ(TUint8 aIrq, TUint64 aApicId, TUint8 aVector, TBool aEnable = EFalse);
  // enable IRQ
  void EnableIRQ(TUint16 aIrq);
  // disable IRQ
  void DisableIRQ(TUint16 aIrq);

  TBool HasIRQ(TUint16 aIrqNumber) {
    dlog("HasIrq irqNumber(%d)) mBase(%d) mMaxRedirectionentries(%d)\n", aIrqNumber, mBase, mMaxRedirectionEntries);
    return aIrqNumber >= mBase && aIrqNumber <= (mBase + mMaxRedirectionEntries);
  }

public:
  TUint32 Id() { return mId; }
  void Id(TUint32 aId) {
    mId = aId & 0x0f;
    WriteRegister(IOAPIC_ID, mId << 24);
  }

protected:
  TUint64 mIndex;
  TUint64 mAddress;
  TUint64 mBase;
  TUint32 mId;
  TUint32 mVersion;
  TUint32 mMaxRedirectionEntries;

protected:
  TUint8 mApicIds[256];
  TUint8 mVectors[256];
};

#endif
