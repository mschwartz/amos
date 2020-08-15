#ifndef EXEX_X86_ACPI_H
#define EXEX_X86_ACPI_H

#include <Types.hpp>
#include <Exec/BBase.hpp>
#include <Exec/x86/ioapic.hpp>
#include <Exec/CPU.hpp>

const TInt MAX_CPUS = 128;
const TInt MAX_IOAPIC = 16;

class IOAPIC;
class CPU;

class ExecBase;

class ACPI : public BBase {
  friend ExecBase;

public:
  ACPI();
  ~ACPI();

public:
  void Dump();

public:
  void EnableIRQ(TUint16 aIrqNumber);
  void DisableIRQ(TUint16 aIrqNumber);
  void AckIRQ(TUint16 aIrqNumber);
  
public:
  TUint64 ApicAddress() {
    return mApicAddress;
  }

protected:
  void ParseSDT(TAny *aSdt, TUint8 aRevision);
  void ParseMADT(TAny *aMadt, TInt32 aLen);

public:
  TUint64 mApicAddress;
  
  // ACPI detected CPUs
  TUint16 mNumCpus;
  // TODO allocate CPUs here
  CPU *mCpus[MAX_CPUS];

  // ACPI deteted IOAPIC info
  TUint16 mNumIoApics;
  IoApic *mIoApics[MAX_IOAPIC];

  // ACPI IRQ Map
  TUint32 mIrqMap[256];
};

#endif
