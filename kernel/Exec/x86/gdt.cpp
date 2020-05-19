#include <Exec/ExecBase.h>
#include <Exec/x86/gdt.h>

const TUint32 GDT_CODE = (0x18 << 8);
const TUint32 GDT_DATA = (0x12 << 8);
const TUint32 GDT_TSS = (0x09 << 8);
const TUint32 GDT_RING0 = (0 << 13);
const TUint32 GDT_RING3 = (3 << 13);
const TUint32 GDT_PRESENT = (1 << 15);
const TUint32 GDT_LONG = (1 << 21);

const TUint32 GDT_AVAILABLE = (1 << 19);
const TUint32 GDT_TSS64 = (1 << 23);

typedef struct {
  TUint16 limit;
  TUint64 base;
} PACKED TGdtPointer;

#if 1
#pragma pack(1)
typedef struct {
  TUint32 : 32;
  TUint64 rsp[3];
  TUint64 : 64;
  TUint64 ist[7];
  TUint64 : 64;
  TUint16 : 16;
  TUint16 iopb;
} PACKED TTss;
#pragma pack(0)
#else
typedef struct {
  TUint32 rsvd0;
  TUint64 rsp0;
  TUint64 rsp1;
  TUint64 rsp2;
  TUint64 rsvd1;
  TUint64 ist1;
  TUint64 ist2;
  TUint64 ist3;
  TUint64 ist4;
  TUint64 ist5;
  TUint64 ist6;
  TUint64 ist7;
  TUint64 rsvd2;
  TUint16 rsvd3;
  TUint16 iopb;
  void Dump() {
    dlog("TTss at %x\n", this);
    dlog("rsvd0: %x\n", rsvd0);
    dlog(" rsp0: %x\n", rsp0);
    dlog(" rsp1: %x\n", rsp1);
    dlog(" rsp2: %x\n", rsp2);
    dlog("rsvd1: %x\n", rsvd1);
    dlog(" ist1: %x\n", ist1);
    dlog(" ist2: %x\n", ist2);
    dlog(" ist3: %x\n", ist3);
    dlog(" ist4: %x\n", ist4);
    dlog(" ist5: %x\n", ist5);
    dlog(" ist6: %x\n", ist6);
    dlog(" ist7: %x\n", ist7);
    dlog("rsvd2: %x\n", rsvd2);
    dlog("rsvd3: %x\n", rsvd3);
    dlog(" iopb: %x\n", iopb);
  }
} PACKED TTss;
#endif

extern "C" void gdt_flush(TGdtPointer *ptr);
extern "C" void ltr();

static TGdtPointer lGdtPointer;

const TInt STACKSIZE = 8192;

static TUint8 stack0[STACKSIZE];
static TUint8 stack1[STACKSIZE];
static TUint8 stack2[STACKSIZE];
static TUint8 stack3[STACKSIZE];
static TUint8 stack4[STACKSIZE];
static TUint8 stack5[STACKSIZE];
static TUint8 stack6[STACKSIZE];
static TUint8 stack7[STACKSIZE];
static TUint8 stack8[STACKSIZE];
static TUint8 stack9[STACKSIZE];

static TTss lTss ALIGN16;

static void DumpTss(TTss &tss) {
  dlog("tss at $016x\n", tss);
  dlog(" rsp0: %016x\n", tss.rsp[0]);
  dlog(" rsp1: %016x\n", tss.rsp[1]);
  dlog(" rsp2: %016x\n", tss.rsp[2]);
  dlog(" ist1: %016x\n", tss.rsp[0]);
  dlog(" ist2: %016x\n", tss.rsp[1]);
  dlog(" ist3: %016x\n", tss.rsp[2]);
  dlog(" ist4: %016x\n", tss.rsp[3]);
  dlog(" ist5: %016x\n", tss.rsp[4]);
  dlog(" ist6: %016x\n", tss.rsp[5]);
  dlog(" ist7: %016x\n", tss.rsp[6]);
}
static void DumpTssSelector(TGdt *p) {
  TUint32 &f = *(TUint32 *)p;
  dlog("base 31:24 %02x g: %d, avl: %d limit 19:16: %x p: %d dpl: %d type: %x base 23:16 %x\n",
    (f >> 24) & 0xff, // base
    (f >> 23) & 1,    // g
    (f >> 19) & 1,    // avl
    (f >> 16) & 0x0f, // limit
    (f >> 15) & 1,    // p
    (f >> 13) & 0x03, // dpl
    (f >> 8) & 0x0f,  // type
    f & 0x0f);        // base 23:16
}

GDT::GDT() {
  dlog("GDT constructor\n");
  //  bochs;
  //  return;
  // The tss is defined in tss.asm, and only a pointer to it is used in this method.
  //  TTss *tss = install_tss();
  //  tss->Dump();
  //  dhexdump(tss, 8);

#if 0
  mGdt[GdtNull] = { 0, 0 };
  mGdt[GdtKCode] = { 0, GDT_PRESENT | GDT_RING0 | GDT_CODE | GDT_LONG };
  mGdt[GdtKData] = { 0, GDT_PRESENT | GDT_RING0 | GDT_DATA };
  mGdt[GdtUCode] = { 0, GDT_PRESENT | GDT_RING3 | GDT_CODE | GDT_LONG };
  mGdt[GdtUData] = { 0, GDT_PRESENT | GDT_RING3 | GDT_DATA };

  mGdt[GdtTssLow] = { 0, 0 };
  mGdt[GdtTssHigh] = { 0, 0 };

  //  mGdt[GdtKData] = GDT_DESCRIPTOR_TYPE | GDT_PRESENT | GDT_RDRW | GDT_64BIT | GDT_RING0;
  //  mGdt[GdtUCode] = GDT_DESCRIPTOR_TYPE | GDT_PRESENT | GDT_RDRW | GDT_64BIT | GDT_RING3 | GDT_EXEC;
  //  mGdt[GdtUData] = GDT_DESCRIPTOR_TYPE | GDT_PRESENT | GDT_RDRW | GDT_64BIT | GDT_RING3;
#else
  TUint64 tss_limit = sizeof(TTss);
  TUint64 tss_base = (TUint64)&lTss;

  // null
  mGdt[GdtNull] = 0;
  // kernel code
  mGdt[GdtKCode] = (1LLU << 44) | (1LLU << 47) | (1LLU << 41LLU) | (1LLU << 43) | (1LLU << 53);
  // kernel data
  mGdt[GdtKData] = (1LLU << 44) | (1LLU << 47) | (1LLU << 41LLU);
  // user code
  mGdt[GdtUCode] = (1LLU << 44) | (1LLU << 47) | (1LLU << 41LLU) | (1LLU << 43) | (1LLU << 53) |
                   (1LLU << 46) | (1LLU << 45);
  // user data
  mGdt[GdtUData] = (1LLU << 44) | (1LLU << 47) | (1LLU << 41LLU) | (1LLU << 46) | (1LLU << 45);
#if 0
  // tss low
  mGdt[GdtTssLow] = ((tss_limit - 1) & 0xffff) |
                    ((tss_base & 0xffffff) << 16) |
                    (0b1001LLU << 40) | (1LLU << 47) |
                    (((tss_base >> 24) & 0xff) << 56);
  // tss high
  mGdt[GdtTssHigh] = tss_base >> 32;
#endif
#endif
  // set up TSS
  SetMemory8(&lTss, 0, sizeof(lTss));
  lTss.rsp[0] = (TUint64)&stack0[STACKSIZE];
  //  lTss.rsp[1] = (TUint64)&stack1[STACKSIZE];
  //  lTss.rsp[2] = (TUint64)&stack2[STACKSIZE];

  //  lTss.ist[0] = (TUint64)&stack3[STACKSIZE];
  //  lTss.ist[1] = (TUint64)&stack4[STACKSIZE];
  //  lTss.ist[2] = (TUint64)&stack5[STACKSIZE];
  //  lTss.ist[3] = (TUint64)&stack6[STACKSIZE];
  //  lTss.ist[4] = (TUint64)&stack7[STACKSIZE];
  //  lTss.ist[5] = (TUint64)&stack8[STACKSIZE];
  //  lTss.ist[6] = (TUint64)&stack9[STACKSIZE];

  lTss.iopb = sizeof(TTss);

//  DumpTss(lTss);

#if 0
  // 64 bit tss setgment in GDT is 2x 64 bit quads
  uint32_t tss_limit = sizeof(TTss);
  uint64_t tss_base = (TUint64)&lTss;

  mGdt[GdtTssLow].flags = GDT_PRESENT | GDT_TSS;
  mGdt[GdtTssLow].flags |= ((tss_base >> 24) & 0xFF) << 24;
  mGdt[GdtTssLow].flags |= (tss_base >> 16) & 0xFF;
  mGdt[GdtTssLow].flags |= ((tss_limit >> 16) & 0xF) << 16;
  mGdt[GdtTssLow].addr = ((tss_base & 0xFFFF) << 16) | (tss_limit & 0xFFFF);
  //
  mGdt[GdtTssHigh].addr = (tss_base >> 32) & 0xFFFFFFFF;
#endif

#if 0
  mGdt[GdtTssLow].flags = GDT_PRESENT | GDT_TSS;

  mGdt[GdtTssLow].flags |= ((tss64 >> 24) & 0xff) << 24;
  mGdt[GdtTssLow].flags |= (tss64 >> 16) & 0xff;
  mGdt[GdtTssLow].flags |= ((limit >> 16) & 0x0f) << 16;
  mGdt[GdtTssLow].addr = ((tss64 & 0xffff) << 16) | (limit & 0xffff);

  mGdt[GdtTssHigh].addr = (tss64 >> 32) & 0xFFFFFFFF;
#endif
  dlog("Tss at %016x\n", &lTss);
//  DumpTssSelector(&mGdt[GdtTssLow]);
  //  mGdt[GdtTssLow].Dump(GdtTssLow);
  //  mGdt[GdtTssHigh].Dump(GdtTssHigh);

  //  DumpTssSelector(&mGdt[GdtTssHigh]);
  //  mGdt[GdtTssHigh].Dump(GdtTssHigh);
  // load the GDT
  lGdtPointer.base = (TUint64)&mGdt[0];
  lGdtPointer.limit = (TUint32)sizeof(mGdt);

  dlog("About to lgdt\n");
  gdt_flush(&lGdtPointer);
  dlog("return from lgdt\n");
//  ltr();
  bochs;
}

GDT::~GDT() {
}

void GDT::SetKernelStack(TUint64 aStackPointer) {
  lTss.rsp[0] = aStackPointer;
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
