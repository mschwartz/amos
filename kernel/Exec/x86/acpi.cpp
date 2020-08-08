#include <Exec/x86/acpi.hpp>
#include <Exec/ExecBase.hpp>
#include <Exec/Memory.hpp>

#define RSDP_SIGNATURE "RSD PTR "
#define MADT_SIGNATURE "APIC"
#define MADT_CPU 0
#define MADT_IOAPIC 1
#define MADT_INT 2

typedef struct {
  TUint8 mSignature[8];
  TUint8 mChecksum;
  TUint8 mOEMID[6];
  TUint8 mRevision;
  TUint32 mRsdt;
  TUint32 mLength;
  TUint64 mXsdt;
  TUint8 mChecksum2;
  TUint8 mReserved[3];
  void Dump() {
    dprint("\n\n");
    dlog("RSDP at %x\n", this);
    dlog(" mSignature: %8s \n", mSignature);
    dlog("  mChecksum: %x\n", mChecksum);
    dlog("  mRevision: %d\n", mRevision);
    dlog("      mRsdt: %x\n", mRsdt);
    dlog("    mLength: %d\n", mLength);
    dlog("      mXsdt: %x\n", mXsdt);
    dlog(" mChecksum2: %x\n", mChecksum2);
    dprint("\n\n");
  }
} PACKED RSDP;

typedef struct {
  TUint8 mSignature[4];
  TUint32 len;
  TUint8 mRevision;
  TUint8 mChecksum;
  TUint8 mOEMID[6];
  TUint8 table_ID[8];
  TUint32 mOemRevision;
  TUint32 creator;
  TUint32 creator_rev;
  TUint8 data[];
  void Dump(TUint8 aRevision) {
    dprint("\n\n");
    dlog("SDT at %x\n", this);
    dlog("     mSignature: %4s\n", mSignature);
    dlog("            len: %d\n", len);
    dlog("       mRevision: %d\n", mRevision);
    dlog("      mChecksum: %x\n", mChecksum);
    dlog("          mOEMID: %6s\n", mOEMID);
    dlog("       table_ID: %8s\n", table_ID);
    dlog("   mOemRevision: %d\n", mOemRevision);
    dlog("        creator: %x\n", creator);
    dlog("     creator_rev: %d\n", creator_rev);

    TUint32 *p32 = (TUint32 *)data;
    TUint64 *p64 = (TUint64 *)data;
    dlog("            data: ");
    for (TInt i = 0; i < 8; i++) {
      if (aRevision) {
        dprint("%016x ", p64[i]);
      }
      else {
        dprint("%08x ", p32[i]);
      }
    }
    dprint("... \n");
    dprint("\n\n");
  }
} PACKED SDT;

typedef struct {
  TUint32 mIoApic;
  TUint32 flags;
  TUint8 data[];
} PACKED MADT;

typedef struct {
  TUint8 type;
  TUint8 len;
  TUint8 data[0];
} PACKED TMadtEntry;

typedef struct {
  TUint8 id;
  TUint8 apic;
  TUint32 flags;
} PACKED TLApic;

typedef struct {
  TUint8 id;
  TUint8 pad;
  TUint32 addr;
  TUint32 base;
} PACKED TIoApic;

typedef struct {
  TUint8 bus;
  TUint8 source;
  TUint32 target;
  uint16_t flags;
} PACKED TInterrupt;

void ACPI::ParseMADT(TAny *aMadt, TInt32 aLen) {
  MADT *madt = (MADT *)aMadt;
  TAny *end = (TAny *)(madt + aLen);

  TMadtEntry *e = (TMadtEntry *)madt->data;

  mIoApic = new IoApic((TUint64)madt->mIoApic);
  dlog("      IO APIC: %x\n", madt->mIoApic);

  while ((TAny *)e < end) {
    TInt i;
    switch (e->type) {
      case MADT_CPU: {
        // APIC descriptor (corresponds to unique cpu core)
        // Check if cpu is enabled
        TLApic *lapic = (TLApic *)&e->data[0];
        dlog("      CPU lapic.id(%x) flags(%x)  ", lapic->id, lapic->flags);
        if (!(lapic->flags & 1)) {
          dprint(" not enabled\n");
          break;
        }
        // Add to list
        dprint(" added\n");
        i = mAcpiInfo.mNumCpus;
        mAcpiInfo.mCpus[i].mId = lapic->id;
        mAcpiInfo.mCpus[i].mApicId = lapic->apic;
        mAcpiInfo.mNumCpus++;
      } break;

      case MADT_IOAPIC: { // IOAPIC descriptor
        i = mAcpiInfo.mNumIoApics;
        TIoApic *ioapic = (TIoApic *)&e->data[0];
        dlog("      ADD IOAPIC(%d)\n", ioapic->id);
        mAcpiInfo.mIoApics[i].mId = ioapic->id;
        mAcpiInfo.mIoApics[i].mAddr = ioapic->addr;
        mAcpiInfo.mIoApics[i].mBase = ioapic->base;
        mAcpiInfo.mNumIoApics++;
      } break;

      case MADT_INT: { // Interrupt remap
        TInterrupt *interrupt = (TInterrupt *)&e->data[0];
        mAcpiInfo.mIrqMap[interrupt->source] = interrupt->target;
      } break;

      default:
        dlog("******************** %x\n", e);
        bochs;
    }
    // dlog(" MADT: type:%d len:%d\n", e->type, e->len);
    if (e->len == 0) {
      break;
    }

    TUint8 *xx = (TUint8 *)e;
    xx += e->len;
    e = (TMadtEntry *)xx;
  }
}

void ACPI::ParseSDT(TAny *aSdt, TUint8 aRevision) {
  SDT *sdt = (SDT *)aSdt;

  // sdt->Dump(revision);

  TUint32 *p32 = (TUint32 *)sdt->data;
  TUint64 *p64 = (TUint64 *)sdt->data;
  int entries = (sdt->len - sizeof(SDT)) / (aRevision ? 8 : 4);
  for (int i = 0; i < entries; i++) {
    SDT *table = (SDT *)(aRevision ? p64[i] : p32[i]);

    dlog("    Found table: (%4s), at %016x table->len (%d)\n", (char *)table->mSignature, table, table->len);
    if (CompareMemory(table->mSignature, MADT_SIGNATURE, 4) == 0) {
      dlog("             MADT\n");
      ParseMADT(table->data, table->len);
    }
  }
}

static TAny *ScanForRSDP(TUint64 aStart, TUint64 aEnd) {
  char *ptr = (char *)aStart;
  while (aStart < aEnd) {
    if (CompareMemory(ptr, RSDP_SIGNATURE, 8) == 0) {
      return (TAny *)ptr;
    }
    ptr += 16;
    aStart += 16;
  }
  return ENull;
}

static TAny *FindRSDP() {
  TSystemInfo info;
  gExecBase.GetSystemInfo(&info);

  // dlog("  trying %x through %x\n", info.mEBDA, info.mEBDA + 1024);
  // dhexdump((const TAny *)0x80000, 64);
  TAny *p = ScanForRSDP(info.mEBDA, info.mEBDA + 1024);
  // TAny *p = ScanForRSDP(0x80000, 0x9ffff);
  if (p) {
    return p;
  }

  // dlog("  trying %x through %x\n", 0x80000, 0x9ffff);
  // p = ScanForRSDP(0x80000, 0x9ffff);
  // dlog("  trying %x through %x\n", 0xe0000, 0xfffff);
  return ScanForRSDP(0xe0000, 0xfffff);
}

ACPI::ACPI() {
  dprint("\n\n");
  dlog("construct ACPI\n");

  RSDP *rsdp = (RSDP *)FindRSDP();
  if (rsdp == ENull) {
    return;
  }
  // rsdp->Dump();

  SDT *s = (SDT *)(rsdp->mRevision ? rsdp->mXsdt : rsdp->mRsdt);
  ParseSDT(s, rsdp->mRevision);
  mAcpiInfo.Dump();
  bochs;
  dprint("\n\n");
}

ACPI::~ACPI() {
  // should neverhappen
  bochs;
}
