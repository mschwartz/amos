#include <Exec/ExecBase.h>
#include <Exec/x86/gdt.h>

const TUint64 GDT_RDRW = (1L << 41);
const TUint64 GDT_CONFORMING = (1L << 42);
const TUint64 GDT_EXEC = (1L << 43);
const TUint64 GDT_DESCRIPTOR_TYPE = (1L << 44); // should be 1 for code and data segments
const TUint64 GDT_RING0(0L << 45);
const TUint64 GDT_RING3(3L << 45);
const TUint64 GDT_PRESENT = (1L << 47);
const TUint64 GDT_64BIT = (1L << 53);
const TUint64 GDT_32BIT = (1L << 54);

#if 0
struct gdt_info {
  TUint16 limit_low;
  TUint16 base_low;
  TUint8 base_middle;
  TUint8 access;
  TUint8 granularity;
  TUint8 base_high;
  void Dump() {
    dprint("gdt_info @ %x (%d bytes long)\n", this, sizeof(gdt_info));
    dprint("limit low %x ", limit_low);
    dprint("base low %x ", base_low);
    dprint("base middle %x ", base_middle);
    dprint("access %x ", access);
    dprint("granularity %x ", granularity);
    dprint("base high %x\n", base_high);
  }
} PACKED;
#endif

typedef struct {
  TUint16 limit;
  TUint64 base;
} PACKED TGdtPointer;

typedef struct {
  TUint32 rsvd0;
  TUint64 rsp0;
  TUint64 rsp1;
  TUint64 rsp2;
  TUint32 rsvd1;
  TUint32 rsvd2;
  TUint64 ist1;
  TUint64 ist2;
  TUint64 ist3;
  TUint64 ist4;
  TUint64 ist5;
  TUint64 ist6;
  TUint64 ist7;
  TUint32 rsvd3;
  TUint32 rsvd4;
  TUint16 rsvd5;
  TUint16 iopb;
  void Dump() {
    dlog("TTss at %x\n", this);
    dlog("rsvd0: %x\n", rsvd0);
    dlog(" rsp0: %x\n", rsp0);
    dlog(" rsp1: %x\n", rsp1);
    dlog(" rsp2: %x\n", rsp2);
    dlog("rsvd1: %x\n", rsvd1);
    dlog("rsvd2: %x\n", rsvd2);
    dlog(" ist1: %x\n", ist1);
    dlog(" ist2: %x\n", ist2);
    dlog(" ist3: %x\n", ist3);
    dlog(" ist4: %x\n", ist4);
    dlog(" ist5: %x\n", ist5);
    dlog(" ist6: %x\n", ist6);
    dlog(" ist7: %x\n", ist7);
    dlog("rsvd3: %x\n", rsvd3);
    dlog("rsvd4: %x\n", rsvd4);
    dlog("rsvd5: %x\n", rsvd5);
    dlog(" iopb: %x\n", iopb);
  }
} PACKED TTss;

extern "C" TTss *install_tss();
extern "C" void gdt_flush(TGdtPointer *ptr);

static TUint64 tss_entry(TUint64 tss) {
  unsigned long long tss_entry = 0x0080890000000067ULL;
  tss_entry |= ((tss) << 16) & 0xffffff0000ULL;
  tss_entry |= ((tss) << 32) & 0xff00000000000000ULL;
  return tss_entry;
}

static TGdtPointer lGdtPointer;

GDT::GDT() {
  dlog("GDT constructor\n");
  bochs;
  return;
  // The tss is defined in tss.asm, and only a pointer to it is used in this method.
  TTss *tss = install_tss();
  tss->Dump();
  dhexdump(tss, 8);
  TUint64 tss64 = (TUint64)tss;

  mGdt[GdtNull] = 0;
  mGdt[GdtKCode] = GDT_DESCRIPTOR_TYPE | GDT_PRESENT | GDT_RDRW | GDT_64BIT | GDT_RING0 | GDT_EXEC;
  mGdt[GdtKData] = GDT_DESCRIPTOR_TYPE | GDT_PRESENT | GDT_RDRW | GDT_64BIT | GDT_RING0;
  mGdt[GdtUCode] = GDT_DESCRIPTOR_TYPE | GDT_PRESENT | GDT_RDRW | GDT_64BIT | GDT_RING3 | GDT_EXEC;
  mGdt[GdtUData] = GDT_DESCRIPTOR_TYPE | GDT_PRESENT | GDT_RDRW | GDT_64BIT | GDT_RING3;
  mGdt[GdtTss] = 0; // tss_entry(tss64) | GDT_64BIT;
  lGdtPointer.base = (TUint64)&mGdt[0];
  lGdtPointer.limit = (TUint32)sizeof(mGdt);

  dlog("About to lgdt\n");
  gdt_flush(&lGdtPointer);
  dlog("return from lgdt\n");
  bochs
}

GDT::~GDT() {
}

#if 0
void GDT::set_gate(TInt aIndex, TAny *aBase, TUint32 aLimit, TUint8 aAccess, TUint8 aGranularity) {
#if 0
	// Fill descriptor
	mGdt[aIndex].BaseLow   = (TUint16)(aBase & 0xFFFF);
	mGdt[aIndex].BaseMid   = (TUint8)((aBase >> 16) & 0xFF);
	mGdt[aIndex].BaseHigh  = (TUint8)((aBase >> 24) & 0xFF);
	mGdt[aIndex].BaseUpper = (TUint32)((aBase >> 32) & 0xFFFFFFFF);
	mGdt[aIndex].LimitLow  = (TUint16)(aLimit & 0xFFFF);
	mGdt[aIndex].Flags     = (TUint8)((aLimit >> 16) & 0x0F);
	mGdt[aIndex].Flags     |= (aGrandularity & 0xF0);
	mGdt[aIndex].Access    = aAccess;
  mGdt[aIndex].Reserved  = 0
#else
  TUint64 addr = (TUint64)aBase;
  ;
  TUint32 gsize = aLimit;
  mGdt[aIndex].limit0 = addr & 0x0000ffff;
  mGdt[aIndex].base_middle = (addr & 0x00ff0000) >> 16;
  mGdt[aIndex].base_high = (addr & 0xff000000) >> 24;

  if (aLimit < (1 << 20)) { /* size < 1 MB? */
    gsize = aLimit - 1;
    mGdt[aIndex].granularity = 0; /* granularity set to 1 byte */
  }
  else {
    gsize = aLimit >> 12;
    if (aLimit & 0x0fff) {
      gsize++;
    }
    gsize--;
    mGdt[aIndex].granularity = 1; /* granularity set to 4 KB */
  }
  mGdt[aIndex].Dump();
  //  gdt[aIndex].DPL = priv_level;

  //  mGdt[aIndex].segm_limit0 = gsize & 0x0000ffff;
  //  mGdt[id].segm_limit1 = (gsize & 0x000f0000) >> 16;
#endif
}
#endif
#if 0
static GDT_t gdtmem[6];
static gdtr gp;
static tss_info tss;

#define SEGNDX_CODE 1
#define SEGNDX_DATA 2
#define SEGNDX_TSS 3

#define PRIV_KERNEL 0
#define PRIV_USER 3

extern "C" void gdt_flush(gdtr *gp);         // ASM function
extern "C" void tss_flush(TUint32 segment); // ASM funct

GDT::GDT() {
  return;
  gp.limit = (sizeof(gdt_info) * 6) - 1;
  gp.base = (TUint64)&gdtmem[0];
//  gp.Dump();
  set_gate(0, 0, 0, 0, 0);                                   // first gate is ALWAYS null
  set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xC0);                    // 0x9A corresponds to ring 0 CODE memory segments
  set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xC0);                    // 0x92 corresponds to  ring 0 DATA memory segments
  set_gate(3, 0, 0xFFFFFFFF, 0xF8, 0xC0);                    // User Code (r3) segment maybe these 2 shouldn't overlap with the ring 0...
  set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xC0);                    // User Data (r3) segment
  set_gate(5, (TUint64)&tss, sizeof(tss_info), 0x89, 0x40); // TSS segment

//  dprint("\n\nabout to flush %x\n", &gp);
  gdt_flush(&gp);
//  tss_install();
}

GDT::~GDT() {
  //
}

void GDT::set_gate(TInt id, void *start_addr, TUint32 size, TUint32 priv_level) {
  TUint64 addr = (TUint64)start_addr;
  TUint32 gsize = size;
  dprint("gsize: %x\n", gsize);
#if 0
  gdtmem[id].limit_low = addr & 0x0000ffff;
  gdtmem[id].base_middle = (addr & 0x00ff0000) >> 16;
  gdtmem[id].base_high = (addr & 0xff000000) >> 24;

  if (size < (1 << 20)) { /* size < 1 MB? */
    gsize = size - 1;
    gdtmem[id].granularity = 0; /* granularity set to 1 byte */
  }
  else {
    gsize = size >> 12;
    if (size & 0x0fff) {
      gsize++;
    }
    gsize--;
    gdtmem[id].granularity = 1; /* granularity set to 4 KB */
  }
  gdtmem[id].Dump();
  //  gdt[id].DPL = priv_level;

  //  gdtmem[id].segm_limit0 = gsize & 0x0000ffff;
  //  gdtmem[id].segm_limit1 = (gsize & 0x000f0000) >> 16;
#endif
}

void GDT::set_gate(TInt id, TUint64 base, TUint64 limit, TUint8 access, TUint8 granularity) {
  GDT_t *g = (GDT_t *)&gdtmem[id];
  TUint32 addr = (TUint32)base;
  TUint32 gsize = limit;

  g->base_addr0 = addr & 0x0000ffff;
  g->base_addr1 = (addr & 0x00ff0000) >> 16;
  g->base_addr2 = (addr & 0xff000000) >> 24;

  if (gsize < (1 << 20)) { /* size < 1 MB? */
    gsize = limit - 1;
    g->G = 0; /* granularity set to 1 byte */
  }
  else {
    gsize = limit >> 12;
    if (limit & 0x0fff)
      gsize++;
    gsize--;
    g->G = 1; /* granularity set to 4 KB */
  }

  g->segm_limit0 = gsize & 0x0000ffff;
  g->segm_limit1 = (gsize & 0x000f0000) >> 16;

  g->DPL = access;
  //  gdtmem[num].base_low = (base & 0xFFFF);
  //  gdtmem[num].base_middle = (base >> 16) & 0xFF;
  //  gdtmem[num].base_high = (base >> 24) & 0xFF;
  //  gdtmem[num].limit_low = (limit & 0xFFFF);
  //  gdtmem[num].granularity = ((limit >> 16) & 0x0F);
  //  gdtmem[num].granularity |= (granularity & 0xF0);
  //  gdtmem[num].access = access;
}

void GDT::tss_install() {
//  tss.ss0 = 0x10; // 0x10 = kernel data segment
//  tss.iomap = sizeof(tss_info);
  tss.iopb = sizeof(tss_info);
  tss_flush(0x28);
}
#endif
