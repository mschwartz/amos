#ifndef EXEX_X86_ACPI_H
#define EXEX_X86_ACPI_H

#include <Exec/Types.h>
#include <Exec/BBase.h>

const TInt MAX_CPUS = 128; // 128 cores max
const TInt MAX_IOAPIC = 4;

class ACPI : public BBase {
public:
  ACPI();
  ~ACPI();

protected:
  void ParseSDT(TAny *std, TUint8 revision) ;

protected:
  // ACPI deteted CPU info
  TUint16 mNumCpus;
  struct {
    TUint8 mId;
    TUint8 mApicId;
  } PACKED mCpus[MAX_CPUS];

  // ACPI deteted IOAPIC info
  TUint16 mNumIoApic;
  struct {
    TUint8 mId;
    TUint32 mAddr;;
    TUint32 mBase;
  } PACKED mIoApic[MAX_IOAPIC];

  // ACPI IRQ Map
  TUint32 mIrqMap[256];
} PACKED;

#endif
