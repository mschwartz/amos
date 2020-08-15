#include <Exec/x86/acpi.hpp>
#include <Exec/ExecBase.hpp>
#include <Exec/Memory.hpp>

const char *RSDP_SIGNATURE =  "RSD PTR ";
const char *MADT_SIGNATURE = "APIC";
const TUint8 MADT_CPU = 0;
const TUint8 MADT_IOAPIC = 1;
const TUint8 MADT_INTERRUPT_SOURCE_OVERRIDE = 2;
const TUint8 MADT_NMI = 4;
const TUint8 MADT_APIC_ADDRESS_OVERRIDE = 5;

void ACPI::Dump() {
  dprint("\n\n");
  dlog("ACPI at %x\n", this);
  dlog("           NumCpus: %d\n", mNumCpus);
  for (TInt c = 0; c < mNumCpus; c++) {
    mCpus[c]->Dump();
    // dlog("          mCpus[%02d]: mId(%d) mApicId(%d)\n", c, mCpus[c].mId, mCpus[c].mApicId);
  }
  dlog("        mNumIoApics: %d\n", mNumIoApics);
  for (TInt a = 0; a < mNumIoApics; a++) {
    mIoApics[a]->Dump();
    // dlog("          mIoApics[%02d]: mId(%d) mAddr(%x) mBase(%x)\n", a, mIoApics[a].mId, mIoApics[a].mAddr, mIoApics[a].mBase);
  }

  dlog("             mIrqMap: ");
  for (TInt i = 0; i < 256; i++) {
    if (i != 0 && (i % 8) == 0) {
      dprint("\n");
      dlog("                      ");
    }
    dprint("%08x ", mIrqMap[i]);
  }
  dprint("\n\n");
  
}

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
  TUint8 mSignature[4];
  TUint32 len;
  TUint8 mRevision;
  TUint8 mChecksum;
  TUint8 mOEMID[6];
  TUint8 table_ID[8];
  TUint32 mOemRevision;
  TUint32 creator;
  TUint32 creator_rev;
  TUint32 lapic_address;
  TUint32 flags;

  // TUint32 mIoApic;
  // TUint32 flags;
  TUint8 data[];
  void Dump() {
    dprint("\n\n");
    dlog("MADT at %x\n", this);
    dlog("     mSignature: %4s\n", mSignature);
    dlog("            len: %d\n", len);
    dlog("       mRevision: %d\n", mRevision);
    dlog("      mChecksum: %x\n", mChecksum);
    dlog("          mOEMID: %6s\n", mOEMID);
    dlog("       table_ID: %8s\n", table_ID);
    dlog("   mOemRevision: %d\n", mOemRevision);
    dlog("        creator: %x\n", creator);
    dlog("     creator_rev: %d\n", creator_rev);
    dlog("   lapic_address: %d\n", lapic_address);
    dlog("           flags: %x\n", flags);
    dprint("\n\n");
  };
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

typedef struct {
  TUint8 mProcessorId;
  TUint16 mFlags;
  TUint8 mLintNumber;
} PACKED TNmi;

typedef struct {
  TInt16 mReserved;
  TInt64 mAddress;
} PACKED TApicOverride;

void ACPI::ParseMADT(TAny *aMadt, TInt32 aLen) {
  MADT *madt = (MADT *)aMadt;
  TAny *end = (TAny *)(madt + aLen);

  mApicAddress = madt->lapic_address;
  dlog("lapic address %x\n", madt->lapic_address);
  TMadtEntry *e = (TMadtEntry *)madt->data;

  while ((TAny *)e < end) {
    TInt i;
    switch (e->type) {

    case MADT_CPU: // APIC descriptor (corresponds to unique cpu core)
      {
	// Check if cpu is enabled
	TLApic *lapic = (TLApic *)&e->data[0];
	dlog("      CPU lapic.id(%x) flags(%x)  ", lapic->id, lapic->flags);
	if (!(lapic->flags & 1)) {
	  dprint(" not enabled\n");
	  break;
	}
	// Add to list
	dprint(" added\n");
	i = mNumCpus;
	CPU *cpu = new CPU(lapic->id, lapic->apic, this);
	mCpus[i] = cpu;
	gExecBase.AddCpu(cpu);
	mNumCpus++;
      }
      break;

    case MADT_IOAPIC: // IOAPIC descriptor
      {
	i = mNumIoApics;
	TIoApic *ioapic = (TIoApic *)&e->data[0];
	dlog("      ADD IOAPIC(%d)\n", ioapic->id);
	mIoApics[i] = new IoApic(mNumIoApics, ioapic->id, ioapic->addr, ioapic->base);
	mNumIoApics++;
      }
      break;

    case MADT_INTERRUPT_SOURCE_OVERRIDE: // Interrupt remap
      {
	TInterrupt *interrupt = (TInterrupt *)&e->data[0];
	dlog("      INTERRUPT SOURCE OVERRIDE %d => %d\n", interrupt->source, interrupt->target);
	mIrqMap[interrupt->source] = interrupt->target;
      }
      break;

    case MADT_NMI: // NMI descriptor
      {
	TNmi *n = (TNmi *)&e->data;
	dlog("      NMI mProcessorId(%x) mFlags(%x) mLintNumber(%d)\n", n->mProcessorId, n->mFlags, n->mLintNumber);
	
      }
      break;

    case MADT_APIC_ADDRESS_OVERRIDE: // APIC ADDRESS OVERRIDE descriptor
      {
	TApicOverride *override = (TApicOverride *)&e->data;
	dlog("      APIC ADDRESS OVERRIDE %x\n", override->mAddress);
	mApicAddress = override->mAddress;
      }
      break;
      
    default:
      dlog("******************** %x\n", e);
      bochs;

    } // switch

    if (e->len == 0) {
      break;
    }

    // set e = next entry
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
      ParseMADT(table, table->len);

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

  // initialize
  mNumCpus = 0;
  for (TInt i = 0; i < MAX_CPUS; i++) {
    mCpus[i] = ENull;
  }

  mNumIoApics = 0;
  for (TInt i = 0; i < MAX_IOAPIC; i++) {
    mIoApics[i] = ENull;
  }

  for (TInt i = 0; i < 256; i++) {
    mIrqMap[i] = 0;
  }

  RSDP *rsdp = (RSDP *)FindRSDP();
  if (rsdp == ENull) {
    return;
  }

  SDT *s = (SDT *)(rsdp->mRevision ? rsdp->mXsdt : rsdp->mRsdt);
  ParseSDT(s, rsdp->mRevision);
  Dump();
  dprint("\n\n");
}

ACPI::~ACPI() {
  // should neverhappen
  bochs;
}

void ACPI::EnableIRQ(TUint16 aIrqNumber) {
  TUint16 irq = aIrqNumber -32;
  for (TInt i=0; i<mNumIoApics; i++) {
    IoApic *io = mIoApics[i];
    if (io->HasIRQ(irq)) {
      io->EnableIRQ(irq);
      return;
    }
  }
  dlog("ACPI Can't enable IRQ(%d) %d\n", aIrqNumber, irq);
}

void ACPI::DisableIRQ(TUint16 aIrqNumber) {
  TUint16 irq = aIrqNumber -32;
  for (TInt i=0; i<mNumIoApics; i++) {
    IoApic *io = mIoApics[i];
    if (io->HasIRQ(irq)) {
      io->DisableIRQ(irq);
      return;
    }
  }
  dlog("ACPI Can't disable IRQ(%d)\n", aIrqNumber);
}

void ACPI::AckIRQ(TUint16 aIrqNumber) {
  CPU *cpu = GetCPU();
  cpu->AckIRQ(aIrqNumber);
}
