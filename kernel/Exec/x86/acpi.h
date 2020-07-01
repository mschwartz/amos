#ifndef EXEX_X86_ACPI_H
#define EXEX_X86_ACPI_H

#include <Types.h>
#include <Exec/BBase.h>

const TInt MAX_CPUS = 128; // 128 cores max
const TInt MAX_IOAPIC = 4;

typedef struct _acpi_ {

  // ACPI detected CPUs
  TUint16 mNumCpus;
  struct {
    TUint8 mId;
    TUint8 mApicId;
  } PACKED mCpus[MAX_CPUS];

  // ACPI deteted IOAPIC info
  TUint16 mNumIoApics;
  struct {
    TUint8 mId;
    TUint32 mAddr;
    TUint32 mBase;
  } PACKED mIoApics[MAX_IOAPIC];

  // ACPI IRQ Map
  TUint32 mIrqMap[256];
public:
  // constructor
  _acpi_() {
    mNumCpus = 0;
    for (TInt i = 0; i < MAX_CPUS; i++) {
      mCpus[i].mId = 0;
      mCpus[i].mApicId = 0;
    }

    mNumIoApics = 0;
    for (TInt i = 0; i < MAX_IOAPIC; i++) {
      mIoApics[i].mId = 0;
      mIoApics[i].mAddr = 0;
      mIoApics[i].mBase = 0;
    }

    for (TInt i = 0; i < 256; i++) {
      mIrqMap[i] = 0;
    }
  }
public:
  void Dump() {
    dprint("\n\n");
    dlog("TAcpiInfo at %x\n", this);
    dlog("           NumCpus: %d\n", mNumCpus);
    for (TInt c=0; c<mNumCpus; c++) {
      dlog("          mCpus[%02d]: mId(%d) mApicId(%d)\n", c, mCpus[c].mId, mCpus[c].mApicId);
    }
    dlog("        mNumIoApics: %d\n", mNumIoApics);
    for (TInt a=0; a<mNumIoApics; a++) {
      dlog("          mIoApics[%02d]: mId(%d) mAddr(%x) mBase(%x)\n", a, mIoApics[a].mId, mIoApics[a].mAddr, mIoApics[a].mBase);
    }

    dlog("             mIrqMap: ");
    for (TInt i=0; i<256; i++) {
      if (i != 0 && (i%8) == 0) {
	dprint("\n");
	dlog("                      ");
      }
      dprint("%08x ", mIrqMap[i]);
    }
    dprint("\n\n");
  }
} PACKED TAcpiInfo;

class ACPI : public BBase {
public:
  ACPI();
  ~ACPI();

protected:
  void ParseSDT(TAny *sdt, TUint8 revision);
  void ParseMADT(TAny *aMadt, TInt32 aLen);

protected:
  TAcpiInfo mAcpiInfo;
};

#endif
