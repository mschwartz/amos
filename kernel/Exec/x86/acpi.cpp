#include <Exec/x86/acpi.h>
#include <Exec/ExecBase.h>
#include <Exec/Memory.h>

#define RSDP_SIGNATURE "RSD PTR "
#define MADT_SIGNATURE "APIC"
#define MADT_CPU 0
#define MADT_IOAPIC 1
#define MADT_INT 2

typedef struct {
  TUint8 signature[8];
  TUint8 checksum;
  TUint8 OEMID[6];
  TUint8 revision;
  TUint32 rsdt;
  TUint32 length;
  TUint64 xsdt;
  TUint8 checksum2;
  TUint8 _[3];
  void Dump() {
    dprint("\n\n");
    dlog("RSDP at %x\n", this);
    dlog("  signature: %8s \n", signature);
    dlog("   checksum: %x\n", checksum);
    dlog("   revision: %d\n", revision);
    dlog("       rsdt: %x\n", rsdt);
    dlog("     length: %d\n", length);
    dlog("       xsdt: %x\n", xsdt);
    dlog("  checksum2: %x\n", checksum2);
    dprint("\n\n");
  }
} PACKED RSDP;

typedef struct {
  TUint8 signature[4];
  TUint32 len;
  TUint8 revision;
  TUint8 checksum;
  TUint8 OEMID[6];
  TUint8 table_ID[8];
  TUint32 OEM_revision;
  TUint32 creator;
  TUint32 creator_rev;
  TUint8 data[];
  void Dump(TUint8 aRevision) {
    dprint("\n\n");
    dlog("SDT at %x\n", this);
    dlog("      signature: %4s\n", signature);
    dlog("            len: %d\n", len);
    dlog("       revision: %d\n", revision);
    dlog("       checksum: %x\n", checksum);
    dlog("          OEMID: %6s\n", OEMID);
    dlog("       table_ID: %8s\n", table_ID);
    dlog("   OEM_revision: %d\n", OEM_revision);
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
  TUint32 lic_address;
  TUint32 flags;
  TUint8 data[];
} PACKED MADT;

typedef struct {
  TUint8 type;
  TUint8 len;
  union {
    struct {
      TUint8 id;
      TUint8 apic;
      TUint32 flags;
    } PACKED lapic;
    struct {
      TUint8 id;
      TUint8 _;
      TUint32 addr;
      TUint32 base;
    } PACKED ioapic;
    struct {
      TUint8 bus;
      TUint8 source;
      TUint32 target;
      uint16_t flags;
    } PACKED interrupt;
  };
} PACKED MADTEntry;

#define incptr(p, n) (TUint64(p) + n)

void ACPI::ParseMADT(TAny *aMadt, TInt32 aLen) {
  MADT *madt = (MADT *)aMadt;
  TAny *end = (TAny *)(madt + aLen);

  MADTEntry *e = (MADTEntry *)madt->data;

  dlog("    Local Interrupt Controller: %x\n", madt->lic_address);

  while ((TAny *)e < end) {
    TInt i;
    switch (e->type) {
      case MADT_CPU: // APIC descriptor (corresponds to unique cpu core)
        // Check if cpu is enabled
        if (!(e->lapic.id & 1)) {
          break;
        }
        // Add to list
        i = mAcpiInfo.mNumCpus;
        mAcpiInfo.mCpus[i].mId = e->lapic.id;
        mAcpiInfo.mCpus[i].mApicId = e->lapic.apic;
        mAcpiInfo.mNumCpus++;
        break;

      case MADT_IOAPIC: // IOAPIC descriptor
        i = mAcpiInfo.mNumIoApics;
        mAcpiInfo.mIoApics[i].mId = e->ioapic.id;
        mAcpiInfo.mIoApics[i].mAddr = e->ioapic.addr;
        mAcpiInfo.mIoApics[i].mBase = e->ioapic.base;
        mAcpiInfo.mNumIoApics++;
        break;

      case MADT_INT: // Interrupt remap
        mAcpiInfo.mIrqMap[e->interrupt.source] = e->interrupt.target;
        break;
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
    e = (MADTEntry *)xx;
  }
}

void ACPI::ParseSDT(TAny *aSdt, TUint8 revision) {
  SDT *sdt = (SDT *)aSdt;

  // sdt->Dump(revision);

  TUint32 *p32 = (TUint32 *)sdt->data;
  TUint64 *p64 = (TUint64 *)sdt->data;
  int entries = (sdt->len - sizeof(SDT)) / (revision ? 8 : 4);
  for (int i = 0; i < entries; i++) {
    SDT *table = (SDT *)(revision ? p64[i] : p32[i]);

    dlog("    Found table: (%4s), at %016x table->len (%d)\n", (char *)table->signature, table, table->len);

    if (CompareMemory(table->signature, MADT_SIGNATURE, 4) == 0) {
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

  SDT *s = (SDT *)(rsdp->revision ? rsdp->xsdt : rsdp->rsdt);
  ParseSDT(s, rsdp->revision);
  mAcpiInfo.Dump();
  dprint("\n\n");
}

ACPI::~ACPI() {
  // should neverhappen
  bochs;
}
