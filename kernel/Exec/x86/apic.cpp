#include <Exec/ExecBase.hpp>
#include <Exec/x86/cpu_utils.hpp>
#include <Exec/x86/apic.hpp>

// https://ethv.net/workshops/osdev/notes-notes-3
// https://github.com/Meulengracht/MollenOS/blob/c3a8d603a6735aa59f0602b4e2aab76340e94a55/kernel/arch/x86/interrupts/apic/apicipi.c

#define LAPIC_MASKED 0x00010000 // Interrupt masked
#define LAPIC_LEVEL 0x00008000  // Level triggered
#define LAPIC_BCAST 0x00080000  // Send to all APICs, including self.
#define LAPIC_INIT 0x00000500   // INIT/RESET
#define LAPIC_ENABLE 0x00000100 // Unit Enable

Apic::Apic(TUint64 aAddress, TUint64 aApicId) {
  dlog("Construct aPic 0x%x\n", aAddress);
  mAddress = aAddress;
  mId = aApicId;
}

TBool Apic::WaitForIdle() {
  TInt timeout = 10;

  while (ReadRegister(APIC_ICR_LOW) & APIC_DELIVERY_BUSY && timeout > 0) {
    --timeout;
  }

  return timeout > 0;
}

// to be called when running in the actual CPU
void Apic::Initialize() {
  dlog("Initialize aPic %d %x (%x)\n", mId, mAddress, ReadRegister(APIC_PROCESSOR_ID) >> 24);

  TUint64 base = read_msr(0x1b);
  dlog("  base=%x %x %s BSP(%s)\n", base, base & ~0xfff,
    base & (1 << 11) ? "true" : "false",
    base & (1 << 8) ? "true" : "false");
  // clear disable bit
  // WriteRegister(APIC_SPURIOUS_REG, LAPIC_ENABLE | 0xff);

  WriteRegister(APIC_SPURIOUS_INTERRUPT_VECTOR, ReadRegister(APIC_SPURIOUS_INTERRUPT_VECTOR) | APIC_ENABLE);

  // set up LINT0 and LINT1
  WriteRegister(APIC_LINT0_REGISTER, 0); // LAPIC_MASKED);
  WriteRegister(APIC_LINT1_REGISTER, 0); // LAPIC_MASKED);

  // clear error status register
  WriteRegister(APIC_ESR, 0);
  WriteRegister(APIC_ESR, 0);

  WriteRegister(APIC_EOI_REGISTER, 0);

  // sync arbitration ids
  WriteRegister(APIC_ICR_HIGH, 0);
  WriteRegister(APIC_ICR_LOW, LAPIC_BCAST | LAPIC_INIT | LAPIC_LEVEL);

  WaitForIdle();

  WriteRegister(APIC_TASK_PRIORITY, 0);
}

TBool Apic::InterruptOthers(TUint8 aVector) {
  DISABLE;

  // dlog("InterruptOthers(%d)\n", aVector);
  if (!WaitForIdle()) {
    dlog("NOT IDLE\n");
    bochs;
    ENABLE;
    return EFalse;
  }

  // WriteRegister(APIC_ICR_HIGH, APIC_ICR_DESTINATION_TYPE_ALL_OTHERS);
  // WriteRegister(APIC_ICR_LOW, APIC_VECTOR(aVector) | APIC_LEVEL_ASSERT | APIC_DESTINATION_PHYSICAL);

  WriteRegister(APIC_ICR_HIGH, 0);
  WriteRegister(APIC_ICR_LOW, APIC_VECTOR(aVector) |
                                APIC_ICR_DESTINATION_TYPE_ALL_OTHERS |
                                APIC_LEVEL_ASSERT |
                                APIC_DESTINATION_PHYSICAL);

  TBool status = WaitForIdle();
  ENABLE;
  return status;
}

TBool Apic::SendIPI(TUint8 aApicId, TUint64 aAddress) {
  DISABLE;
  // dlog("SendIPI(%d, %x)\n", aApicId, aAddress);
  if (!WaitForIdle()) {
    // dlog("SendIPI %d not idle\n", aApicId);
    ENABLE;
    return EFalse;
  }

  WriteRegister(APIC_ICR_HIGH, APIC_DESTINATION(aApicId));
  WriteRegister(APIC_ICR_LOW, APIC_DELIVERY_MODE_INIT | APIC_LEVEL_ASSERT | APIC_DESTINATION_PHYSICAL);

  TBool status = WaitForIdle();
  // dlog("SendIPI (%d, %x) returns %d\n", aApicId, aAddress, status);
  ENABLE;
  return status;
}

TBool Apic::SendSIPI(TUint8 aApicId, TUint64 aAddress) {
  DISABLE;
  if (!WaitForIdle()) {
    ENABLE;
    return EFalse;
  }

  WriteRegister(APIC_ICR_HIGH, APIC_DESTINATION(aApicId));
  WriteRegister(APIC_ICR_LOW, APIC_DELIVERY_MODE_SIPI | APIC_LEVEL_ASSERT | APIC_DESTINATION_PHYSICAL | (aAddress >> 12));

  TBool status = WaitForIdle();
  ENABLE;
  return status;
}
