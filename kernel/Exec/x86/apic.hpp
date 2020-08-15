#ifndef AMOS_KERNEL_EXEC_X86_APIC_HPP
#define AMOS_KERNEL_EXEC_X86_APIC_HPP

// https://ethv.net/workshops/osdev/notes-notes-3

#include <Types.hpp>

const TUint32 APIC_PROCESSOR_ID = 0x20;
const TUint32 APIC_VERSION = 0x30;
const TUint32 APIC_EOI_REGISTER = 0xB0;
const TUint32 APIC_TASK_PRIORITY = 0x80;
const TUint32 APIC_LOGICAL_DEST = 0xd0;
const TUint32 APIC_DEST_FORMAT = 0xe0;
const TUint32 APIC_SPURIOUS_INTERRUPT_VECTOR = 0xf0;
const TUint32 APIC_ESR = 0x280;
const TUint32 APIC_CMCI = 0x2F0;
const TUint32 APIC_ICR_LOW = 0x300;
const TUint32 APIC_ICR_HIGH = 0x310;
const TUint32 APIC_TIMER_VECTOR = 0x320;
const TUint32 APIC_THERMAL_SENSOR = 0x330;
const TUint32 APIC_PERF_MONITOR = 0x340;
const TUint32 APIC_LINT0_REGISTER = 0x350;
const TUint32 APIC_LINT1_REGISTER = 0x360;
const TUint32 APIC_ERROR_REGISTER = 0x370;
const TUint32 APIC_INITIAL_COUNT = 0x380;
const TUint32 APIC_CURRENT_COUNT = 0x390;
const TUint32 APIC_DIVIDE_REGISTER = 0x3e0;

// ICR HIGH WORD bits
#define APIC_DESTINATION(Dest) ((Dest &= 0x7F) << 24)

const TUint32 APIC_DESTINATION_PHYSICAL = 0;
const TUint32 APIC_DESTINATION_LOGICAL(1 << 11);
const TUint32 APIC_DELIVERY_BUSY(1 << 12);
const TUint32 APIC_LEVEL_ASSERT(1 << 14);
const TUint32 APIC_TRIGGER_EDGE = 0;
const TUint32 APIC_TRIGGER_LEVEL(1 << 15);
const TUint32 APIC_BROADCAST = 0xFF000000;


// ICR LOW WORD bits (APIC offset 0x300)
// the vector number
#define APIC_VECTOR(Vector) (Vector &= 0xFF)

// bits 8-10 of ICR (DELIVERY MODE)
const TUint32 APIC_DELIVERY_MODE_FIXED = 0x00 << 8;
const TUint32 APIC_DELIVERY_MODE_LOWEST_PRIORITY = 0x01 << 8;
const TUint32 APIC_DELIVERY_MODE_SMI = 0x02 << 8;
const TUint32 APIC_DELIVERY_MODE_NMI = 0x04 << 8;
const TUint32 APIC_DELIVERY_MODE_INIT = 0x05 << 8;
const TUint32 APIC_DELIVERY_MODE_SIPI = 0x06 << 8;
const TUint32 APIC_DELIVERY_MODE_EXTINT = 0x07 << 8;

const TUint32 APIC_ICR_DESTINATION_TYPE_NONE = 0x00 << 18;
const TUint32 APIC_ICR_DESTINATION_TYPE_SELF = 0x01 << 18;
const TUint32 APIC_ICR_DESTINATION_TYPE_ALL = 0x01 << 19;
const TUint32 APIC_ICR_DESTINATION_TYPE_ALL_OTHERS = 0x03 << 18;

const TUint32 APIC_ENABLE = (1<<8);

class CPU;
class Apic {
  friend CPU;
public:
  Apic(TUint64 aAddress, TUint64 aApicId);

  void Initialize();

public:
  void Dump() {
    dlog("APIC(%x) address(%x)\n", this, mAddress);
  }


public:
  void WriteRegister(TUint32 aOffset, TUint32 aValue) {
    *(volatile TUint32 *)(mAddress + aOffset) = aValue;
  }

  TUint32 ReadRegister(TUint32 aOffset) {
    TUint32 val = *(volatile TUint32 *)(mAddress + aOffset);
    return val;
  }

protected:
  TBool WaitForIdle();

public:
  TUint64 Address() {
    return mAddress;
  }

  TUint32 ApicId() {
    return ReadRegister(APIC_PROCESSOR_ID);
  }

  void EOI(TUint64 aIrq) {
    WriteRegister(APIC_EOI_REGISTER, 0);
  }

public:
  TBool InterruptOthers(TUint8 aVector);
  TBool SendIPI(TUint8 aApicId, TUint64 aAddress);
  TBool SendSIPI(TUint8 aApicId, TUint64 aAddress);

protected:
  TUint64 mAddress;
  TUint64 mId;
};

#endif
