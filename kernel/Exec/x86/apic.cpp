#include <Exec/ExecBase.hpp>
#include <Exec/x86/cpu_utils.hpp>
#include <Exec/x86/apic.hpp>

// https://ethv.net/workshops/osdev/notes-notes-3
// https://github.com/Meulengracht/MollenOS/blob/c3a8d603a6735aa59f0602b4e2aab76340e94a55/kernel/arch/x86/interrupts/apic/apicipi.c

Apic::Apic(TUint64 aAddress, TUint64 aApicId) {
  dlog("Construct aPic\n");
  mAddress = aAddress;
  mId = aApicId;
  TUint32 val = ReadRegister(APIC_SPURIOUS_REG);
  val |= (1 << 8);
  WriteRegister(APIC_SPURIOUS_REG, val);
}

TBool Apic::WaitForIdle() {
  TInt timeout = 10;

  while (ReadRegister(APIC_ICR_LOW) & APIC_DELIVERY_BUSY && --timeout > 0) {
  }

  return timeout > 0;
}

TBool Apic::InterruptOthers(TUint8 aVector) {
  DISABLE;
  if (!WaitForIdle()) {
    ENABLE;
    return EFalse;
  }

  WriteRegister(APIC_ICR_HIGH, APIC_ICR_SH_ALL_OTHERS);
  WriteRegister(APIC_ICR_LOW, APIC_VECTOR(aVector) | APIC_LEVEL_ASSERT | APIC_DESTINATION_PHYSICAL);

  TBool status = WaitForIdle();
  ENABLE;
  return status;
}

TBool Apic::SendIPI(TUint8 aApicId, TUint64 aAddress) {
  DISABLE;
  dprint("\n\n");
  dlog("SendIPI(%d, %x)\n", aApicId, aAddress);
  if (!WaitForIdle()) {
    dlog("SendIPI %d not idle\n", aApicId);
    ENABLE;
    return EFalse;
  }

  WriteRegister(APIC_ICR_HIGH, APIC_DESTINATION(aApicId));
  WriteRegister(APIC_ICR_LOW, APIC_DELIVERY_MODE(APIC_MODE_INIT) | APIC_LEVEL_ASSERT | APIC_DESTINATION_PHYSICAL);

  TBool status = WaitForIdle();
  dlog("SendIPI (%d, %x) returns %d\n", aApicId, aAddress, status);
  ENABLE;
  return status;
}

TBool Apic::SendSIPI(TUint8 aApicId, TUint64 aAddress) {
  DISABLE;
  dprint("\n\n");
  dlog("SendSIPI(%d, %x)\n", aApicId, aAddress);
  if (!WaitForIdle()) {
    dlog("SendSIPI %d not idle\n", aApicId);
    ENABLE;
    return EFalse;
  }

  WriteRegister(APIC_ICR_HIGH, APIC_DESTINATION(aApicId));
  WriteRegister(APIC_ICR_LOW, APIC_DELIVERY_MODE(APIC_MODE_SIPI) | APIC_LEVEL_ASSERT | APIC_DESTINATION_PHYSICAL | (aAddress>>12));
  TBool status = WaitForIdle();
  dlog("SendSIPI (%d, %x) returns %d\n", aApicId, aAddress, status);
  ENABLE;
  return status;
}
