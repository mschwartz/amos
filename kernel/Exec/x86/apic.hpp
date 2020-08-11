#ifndef AMOS_KERNEL_EXEC_X86_APIC_HPP
#define AMOS_KERNEL_EXEC_X86_APIC_HPP

// https://ethv.net/workshops/osdev/notes-notes-3

#include <Types.hpp>

const TUint8 APIC_ID = 0x020;
const TUint8 APIC_EOI = 0xb0;
const TUint8 APIC_SPURIOUS_INTERRUPT = 0xf0;

#define APIC_PROCESSOR_ID 0x20
#define APIC_VERSION 0x30
#define APIC_TASK_PRIORITY 0x80
#define APIC_INTERRUPT_ACK 0xB0
#define APIC_LOGICAL_DEST 0xD0
#define APIC_DEST_FORMAT 0xE0
#define APIC_SPURIOUS_REG 0xF0
#define APIC_ESR 0x280
#define APIC_CMCI 0x2F0
#define APIC_ICR_LOW 0x300
#define APIC_ICR_HIGH 0x310
#define APIC_TIMER_VECTOR 0x320
#define APIC_THERMAL_SENSOR 0x330
#define APIC_PERF_MONITOR 0x340
#define APIC_LINT0_REGISTER 0x350
#define APIC_LINT1_REGISTER 0x360
#define APIC_ERROR_REGISTER 0x370
#define APIC_INITIAL_COUNT 0x380
#define APIC_CURRENT_COUNT 0x390
#define APIC_DIVIDE_REGISTER 0x3E0

#define APIC_VECTOR(Vector) (Vector & 0xFF)
#define APIC_DELIVERY_MODE(Mode) ((Mode & 0x7) << 8)
#define APIC_DESTINATION_PHYSICAL 0
#define APIC_DESTINATION_LOGICAL (1 << 11)
#define APIC_DELIVERY_BUSY (1 << 12)
#define APIC_LEVEL_ASSERT (1 << 14)
#define APIC_TRIGGER_EDGE 0
#define APIC_TRIGGER_LEVEL (1 << 15)
#define APIC_DESTINATION(Dest) ((Dest & 0x7F) << 24)
#define APIC_BROADCAST 0xFF000000

#define APIC_MODE_FIXED 0x0
#define APIC_MODE_LOWEST_PRIORITY 0x1
#define APIC_MODE_SMI 0x2
#define APIC_MODE_NMI 0x4
#define APIC_MODE_INIT 0x5
#define APIC_MODE_SIPI 0x6
#define APIC_MODE_EXTINT 0x7

#define APIC_ICR_SH_NONE 0
#define APIC_ICR_SH_SELF (1 << 18)
#define APIC_ICR_SH_ALL (1 << 19)
#define APIC_ICR_SH_ALL_OTHERS (1 << 18) | (1 << 19)

class Apic {
public:
  Apic(TUint64 aAddress, TUint64 aApicId);

public:
  void Dump() {
    dlog("APIC(%x) address(%x)\n", this, mAddress);
  }

public:
  void WriteRegister(TUint32 aOffset, TUint32 aValue) {
    dlog("    > WriteRegister mAddress(%x) %x (%x) <= %x\n", mAddress, mAddress + aOffset, aOffset, aValue);
    *(volatile TUint32 *)(mAddress + aOffset) = aValue;
  }

  TUint32 ReadRegister(TUint32 aOffset) {
    TUint32 val = *(volatile TUint32 *)(mAddress + aOffset);
    dlog("    < ReadRegister %x (%x) = %x\n", mAddress + aOffset, aOffset, val);
    return val;
  }

protected:
  TBool WaitForIdle();

public:
  TUint64 Address() { return mAddress; }
  TUint32 ApicId() { return ReadRegister(APIC_PROCESSOR_ID); }
  void EOI() { WriteRegister(APIC_INTERRUPT_ACK, 1); }

public:
  TBool InterruptOthers(TUint8 aVector);
  TBool SendIPI(TUint8 aApicId, TUint64 aAddress);
  TBool SendSIPI(TUint8 aApicId, TUint64 aAddress);

protected:
  TUint64 mAddress;
  TUint64 mId;
};

#endif
