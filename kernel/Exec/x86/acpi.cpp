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

static TAny *ScanForRSDP(TUint64 aStart, TUint64 aEnd) {
  while (aStart < aEnd) {
    if (CompareMemory((char *)aStart, RSDP_SIGNATURE, 8) == 0) {
      return (TAny *)aStart;
    }
    aStart += 16;
  }
  return ENull;
}

static TAny *FindRSDP() {
  TSystemInfo info;
  gExecBase.GetSystemInfo(&info);

  TAny *p = ScanForRSDP(info.mEBDA, info.mEBDA+1024);
  if (p) {
    return p;
  }
  return ScanForRSDP(0xe0000, 0xfffff);
}

void ACPI::ParseSDT(TAny *aStd, TUint8 revision) {
  SDT *sdt = (SDT *)aStd;

  TUint32 *p32 = (TUint32 *)sdt->data;
  TUint64 *p64 = (TUint64 *)sdt->data;
  int entries = (sdt->len - sizeof(SDT)) / (revision ? 8 : 4);
  for(int i = 0; i < entries; i++) {
    SDT *table = (SDT *)(revision ? p64[i] : p32[i]);

    dlog("Found table: %s\n", (char *)table->signature);

    if(CompareMemory(table->signature, MADT_SIGNATURE, 4) == 0) {
      // parse_madt((void *)table->data, table->len);
    }
  }
  
}
ACPI::ACPI() {
  dlog("construct ACPI\n");

  mNumCpus = 0;
  for (TInt i = 0; i < MAX_CPUS; i++) {
    mCpus[i].mId = 0;
    mCpus[i].mApicId = 0;
  }

  mNumIoApic = 0;
  for (TInt i = 0; i < MAX_IOAPIC; i++) {
    mIoApic[i].mId = 0;
    mIoApic[i].mAddr = 0;
    mIoApic[i].mBase = 0;
  }

  for (TInt i = 0; i < 256; i++) {
    mIrqMap[i] = 0;
  }

  RSDP *rsdp = (RSDP *)FindRSDP();
  if (rsdp == ENull) {
    return;
  }

  SDT*s = (SDT *)(rsdp->revision ? rsdp->xsdt : rsdp->rsdt);
}

ACPI::~ACPI() {
  // should neverhappen
  bochs;
}

#if 0
#include <acpi.h>
#include <memory.h>
#include <debug.h>


struct acpi_info acpi_info = {0};

static void *scan_rsdp(TUint64 start, TUint64 end)
{
  void *p = P2V(start);
  while(p < P2V(end))
    {
      if(!memcmp(p, RSDP_SIGNATURE, 8))
	return p;
      p = incptr(p, 16);
    }
  return 0;
}

static struct rsdp *find_rsdp()
{
  // Scan the Extended BIOS Data Area
  uint16_t *ebda_ptr = P2V(0x40e);
  TUint64 ebda = *ebda_ptr << 4;
  void *p = scan_rsdp(ebda, ebda+1024);
  if(p) return p;

  // Scan 0xE0000 - 0xFFFFF
  p = scan_rsdp(0xE0000, 0xFFFFF);
  if(p) return p;

  return 0;
}

static void parse_madt(struct madt *madt, TUint32 len)
{
  uintptr_t end = (uintptr_t)madt + len;
  struct madt_entry *e = (void *)madt->data;
  debug_info("Local Interrupt Controller: %x\n", madt->lic_address);
  while((uintptr_t)e < end)
    {
      int i;
      switch(e->type)
	{
	case MADT_CPU: // APIC descriptor (corresponds to unique cpu core)
	  // Check if cpu is enabled
	  if(!(e->lapic.id & 1)) break;
	  // Add to list
	  i = acpi_info.num_cpus;
	  acpi_info.cpu[i].id = e->lapic.id;
	  acpi_info.cpu[i].apic = e->lapic.apic;
	  acpi_info.num_cpus++;
	  break;

	case MADT_IOAPIC: // IOAPIC descriptor
	  i = acpi_info.num_ioapic;
	  acpi_info.ioapic[i].id = e->ioapic.id;
	  acpi_info.ioapic[i].addr = e->ioapic.addr;
	  acpi_info.ioapic[i].base = e->ioapic.base;
	  acpi_info.num_ioapic++;
	  break;

	case MADT_INT: // Interrupt remap
	  acpi_info.int_map[e->interrupt.source] = e->interrupt.target;
	  break;
	}
      debug_info(" MADT: type:%d len:%d\n", e->type, e->len);
      e = incptr(e, e->len);
    }
}

static void parse_sdt(struct sdt *sdt, TUint8 revision)
{
  TUint32 *p32 = (void *)sdt->data;
  TUint64 *p64 = (void *)sdt->data;
  int entries = (sdt->len - sizeof(struct sdt)) / (revision ? 8 : 4);
  for(int i = 0; i < entries; i++)
    {
      struct sdt *table = P2V(revision ? p64[i] : p32[i]);

      debug_info("Found table: ");
      debug_putsn((char *)table->signature, 4);
      debug_printf("\n");

      if(!memcmp(table->signature, MADT_SIGNATURE, 4))
	parse_madt((void *)table->data, table->len);
    }
}

void acpi_init()
{
  struct rsdp *rsdp = find_rsdp();
  struct sdt *s = P2V(rsdp->revision ? rsdp->xsdt : rsdp->rsdt);
  parse_sdt(s, rsdp->revision);
}
#endif
