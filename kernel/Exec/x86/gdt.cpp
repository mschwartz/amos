#include <Exec/x86/cpu.h>
#include <Exec/x86/gdt.h>
#include <Exec/Memory.h>

/*
GDT Entry:

 3              1 1
 1              6 5              0
+----------------+----------------+
| Base 0:15      | Limit 0:15     |
+----------------+----------------+

 6      5 5  5 5  4  4      4 3       3
 3      6 5  2 1  8  7      0 9       2
+--------+-----+-----+--------+--------+
| Base   |FLAGS|LIMIT| ACCESS | BASE   |
| 24:31  |     |16:19|        | 16:23  |
+--------+-----+-----+--------+--------+

Flags:
GR      0x08 Granularity: 0 = bytes, 1 = 4K bytes
Sz      0x04 0 = 16 bit selector, 1 = 32 bit selector
L       0x02 0 = 32 bit descriptor, 1 = 64 bit (Sz must be 0 if 64 bit)

Access:
Present 0x80
DPL     0x20 2 bits, 0 or 3
S       0x10 Descriptor type: 1 = code, 0 = data/tss
Ex      0x08 Executable bit: 1 = code selector
DC      0x04 Data selector direction: grows 0=up,1=down   
             Code Selctor Conforming: 0 code can only be executed in same ring, 1 code in ring <= current
RW      0x02 R/W 1 = read access for code selector, 1 = write access for data selector
AC      0x01 Accessed (set to 0) CPU sets this when segment is accessed (e.g. busy)
 */

extern "C" void gdt_flush(TAny *gp);        // ASM function
extern "C" void tss_flush(TUint32 segment); // ASM funct

#if 1
enum GdtFlags : TUint32 {
  kGdtWord1CodeSegment = 0x1800,
  kGdtWord1DataSegment = 0x1200,
  kGdtWord1TssDescriptor = 0x0900,
  kGdtWord1User = 0x6000,
  kGdtWord1Present = 0x8000,
  kGdtWord1Long = 0x00200000,
  kGdtWord1Default = 0x00400000,
  kGdtWord1Granularity = 0x00800000
};

#if 0
struct Gdtr {
  TUint16 limit;
  TUint32 *pointer;
};
#endif

#if 1
#define GDT_CODE (0x18L << 8)
#define GDT_DATA (0x12L << 8)
#define GDT_TSS (0x89L << 8)
#define GDT_DPL(lvl) ((lvl) << 13)
#define GDT_PRESENT (1L << 15)
#define GDT_LONG (1L << 21)

struct tss {
  TUint32 r1;
  TUint64 rsp0;
  TUint64 rsp1;
  TUint64 rsp2;
  TUint64 r2;
  TUint64 ist1;
  TUint64 ist2;
  TUint64 ist3;
  TUint64 ist4;
  TUint64 ist5;
  TUint64 ist6;
  TUint64 ist7;
  TUint64 r3;
  TUint16 r4;
  TUint16 io_mba;
} PACKED;

#if 0
struct gdt {
  TUint32 addr;
  TUint32 flags;
} PACKED;
struct gdtp {
  TUint16 len;
  struct gdt *gdt;
} PACKED;
struct gdt BootGDT[] = {
  /* 0 */ { 0, 0 },                                              // 0
  /* 1 */ { 0, GDT_PRESENT | GDT_DPL(0) | GDT_CODE | GDT_LONG }, // 8
  /* 2 */ { 0, GDT_PRESENT | GDT_DPL(0) | GDT_DATA },            // 0x10
  /* 3 */ { 0, GDT_PRESENT | GDT_DPL(3) | GDT_CODE | GDT_LONG }, // 0x18
  /* 4 */ { 0, GDT_PRESENT | GDT_DPL(3) | GDT_DATA },            // 0x20
  /* 5 */ { 0, 0 },                                              // 0x28 TSS is two entries
  /* 6 */ { 0, 0 },                                              // 0x30
  /* 7 */ { 0, 0 },                                              // 0x38 (Unused)
};

#else
struct gdtp {
  TUint16 len;
  TUint64 *gdt;
} PACKED;
TUint64 BootGDT[] = {
  /* 0 */ 0,                                                      // 0
  /* 1 */ (GDT_PRESENT | GDT_DPL(0) | GDT_CODE | GDT_LONG) << 32, // 8
  /* 2 */ (GDT_PRESENT | GDT_DPL(0) | GDT_DATA) << 32,            // 0x10
  /* 3 */ (GDT_PRESENT | GDT_DPL(3) | GDT_CODE | GDT_LONG) << 32, // 0x18
  /* 4 */ (GDT_PRESENT | GDT_DPL(3) | GDT_DATA) << 32,            // 0x20
  /* 5 */ 0,                                                      // 0x28 TSS is two entries
  /* 6 */ 0,                                                      // 0x30
  /* 7 */ 0,                                                      // 0x38 (Unused)
};
#endif

struct gdtp GDTp = { 2 * 8 - 1, BootGDT };

TUint64 gGdt[8] ALIGN16;
struct tss gTss ALIGN16;

#define KSTACK_SIZE (2 * 1024 * 1024)

TUint8 gKStack1[KSTACK_SIZE] ALIGN16;

static void PrintSelector(TUint64 aSelector) {
  dprint("Selector %016x\n", aSelector);
  dprint("Access: %x\n", (aSelector >> 40) & 0xff);
}

GDT::GDT() {
  // void gdt_init(struct cpu *c) {
  // struct gdt *gdt = (struct gdt *)&gGdt[0];
  TUint64 *gdt = (TUint64 *)&gdt[0];
  CopyMemory(gdt, BootGDT, sizeof(BootGDT));

  struct tss *tss = (struct tss *)&gTss;
  SetMemory8(tss, 0, sizeof(struct tss));
  tss->io_mba = sizeof(struct tss);
  tss->rsp0 = (TUint64)&gKStack1[KSTACK_SIZE];

  TUint32 tss_limit = sizeof(struct tss);
  TUint64 tss_base = (TUint64)tss;

  // TSS
  const int SEG_TSS = 5;

  TUint64 addr = tss_base;
  gdt[SEG_TSS + 0] = (0x0067) | ((addr & 0xFFFFFF) << 16) | (0x00E9LL << 40) |
                     (((addr >> 24) & 0xFF) << 56);
  gdt[SEG_TSS + 1] = (addr >> 32);

  dprint("TSS SELECTOR %016x ", gdt[SEG_TSS]);
  dhex64(gdt[SEG_TSS]);
  dprint("\n");
  dprint("Access: %x\n", (gdt[SEG_TSS] >> 40) & 0xff);
  // gdt[5]= (GDT_PRESENT | GDT_TSS) << 32;
  // gdt[5] |= (((tss_base >> 24) & 0xFF) << 24) << 32;
  // gdt[5] |= ((tss_base >> 16) & 0xFF) << 32;
  // gdt[5] |= (((tss_limit >> 16) & 0xF) << 16) << 32;
  // gdt[5] |= ((tss_base & 0xFFFF) << 16) | (tss_limit & 0xFFFF);

  // gdt[6] = 0;
  // gdt[6].flags = 0;
  // gdt[6].addr = (tss_base >> 32) & 0xFFFFFFFF;

  dprint("tss_base: %016x limit: %d(%x)\n", tss_base, tss_limit);
  dhexdump64(gGdt, 1);
  // dlog("Print Selector %016x %016x\n", gGdt[5], gGdt[6]);
  PrintSelector(gGdt[SEG_TSS]);
  // for (TInt i=0; i<8; i++) {
  //   dprint("%d: %016x/%016x %016x/%08x %016x/%08x\n", i, &gGdt[i], gGdt[i], &gdt[i].addr, gdt[i].addr, &gdt[i].flags, gdt[i].flags);
  // }

  GDTp.len = 6 * 8 - 1;
  GDTp.gdt = gdt;

  gdt_flush(&GDTp);
  // tss_flush(0x28);
  // load_gdt(&GDTp);
}

#endif
#endif

////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
//
//
//
//
//

#if 0
// chariot style

#define SEG_NULL 0
#define SEG_KCODE 1
#define SEG_KDATA 2
#define SEG_UCODE 3
#define SEG_UDATA 4
#define SEG_TSS 5

#define PGSIZE 4096
struct gdt_desc64 {
  TUint16 limit;
  TUint64 base;
} __packed;

static inline void lgdt(void *data, int size) {
  gdt_desc64 gdt;
  gdt.limit = size - 1;
  gdt.base = (TUint64)data;

  asm volatile("lgdt %0" ::"m"(gdt));
}

static inline void ltr(TUint16 sel) { asm volatile("ltr %0"
                                                   :
                                                   : "r"(sel)); }
struct tss {
  TUint32 r1;
  TUint64 rsp0;
  TUint64 rsp1;
  TUint64 rsp2;
  TUint64 r2;
  TUint64 ist1;
  TUint64 ist2;
  TUint64 ist3;
  TUint64 ist4;
  TUint64 ist5;
  TUint64 ist6;
  TUint64 ist7;
  TUint64 r3;
  TUint16 r4;
  TUint16 io_mba;
} PACKED;

#define KSTACKSIZE 65536
static TUint8 kstack[KSTACKSIZE];
// static CPU cpu;

TUint64 gdt[8];
struct tss tss;

gdt_desc64 gdtr;

GDT::GDT() {
  // TUint8 *local = cpu.mLocal;
  // TUint64 *gdt = (TUint64 *)local;

  // TUint32 *tss = (TUint32 *)&local[1024];
  TUint64 addr = (TUint64)&tss;

  tss.io_mba = 0x68;
  tss.rsp0 = (TUint64)&kstack[KSTACKSIZE];
  // tss[16] = 0x00680000; // IO Map Base = End of TSS
  // tss[0x64] |= (0x64 * sizeof(TUint32)) << 16;

  // wrmsr(0xC0000100, ((TUint64)local) + (PGSIZE / 2));

  gdt[SEG_NULL] = 0x0000000000000000;  // NULL
  gdt[SEG_KCODE] = 0x0020980000000000; // Code, DPL=0, R/X
  gdt[SEG_KDATA] = 0x0000920000000000; // Data, DPL=0, W
  gdt[SEG_UCODE] = 0x0020F80000000000; // Code, DPL=3, R/X
  gdt[SEG_UDATA] = 0x0000F20000000000; // Data, DPL=3, W
  // gdt[SEG_TSS + 0] = (0x0067) | ((addr & 0xFFFFFF) << 16) | (0x00E9LL << 40) | (((addr >> 24) & 0xFF) << 56);
  // gdt[SEG_TSS + 1] = (addr >> 32);

  dhexdump64(gdt, 1);
  gdtr.limit = 8 * sizeof(TUint64) - 1;
  gdtr.base = (TUint64)gdt;
  gdt_flush(&gdtr);
  // lgdt((void *)gdt, 8 * sizeof(TUint64));
}

#endif
#if 0
#define GDT_MAX_TSS 128
#define GDT_MAX_DESCRIPTORS (GDT_MAX_TSS + 8)
#define GDT_IOMAP_SIZE ((0xFFFF / 8) + 1)

/* 8 Hardcoded system descriptors, we must have a 
 * null descriptor to catch cases where segment has
 * been set to 0, and we need 2 for ring 0, and 4 for ring 3 */
#define GDT_NULL_SEGMENT 0x00
#define GDT_KCODE_SEGMENT 0x10 // Kernel
#define GDT_KDATA_SEGMENT 0x20 // Kernel
#define GDT_UCODE_SEGMENT 0x30 // Applications
#define GDT_UDATA_SEGMENT 0x40 // Applications
#define GDT_EXTRA_SEGMENT 0x50 // Shared

/* Gdt type codes, they set the appropriate bits
 * needed for our needs, both for code and data segments
 * where kernel == ring0 and user == ring3 */
#define GDT_GRANULARITY 0x20
#define GDT_RING0_CODE 0x9A
#define GDT_RING0_DATA 0x92
#define GDT_RING3_CODE 0xFA
#define GDT_RING3_DATA 0xF2
#define GDT_TSS_ENTRY 0xE9

/* The GDT access flags, they define general information
 * about the code / data segment */

/* Data Access */
#define GDT_ACCESS_WRITABLE 0x02
#define GDT_ACCESS_DOWN 0x04 /* Grows down instead of up */

/* Code Access */
#define GDT_ACCESS_READABLE 0x02
#define GDT_ACCESS_CONFORMS 0x04

/* Shared Access */
#define GDT_ACCESS_ACCESSED 0x01
#define GDT_ACCESS_EXECUTABLE 0x08
#define GDT_ACCESS_RESERVED 0x10
#define GDT_ACCESS_PRIV3 (0x20 | 0x40)
#define GDT_ACCESS_PRESENT 0x80

#define GDT_FLAG_32BIT 0x40
#define GDT_FLAG_PAGES 0x80

struct GdtObject {
  TUint16 Limit;
  TUint64 Base;
} PACKED;

struct GdtDescriptor {
  TUint16 LimitLow;
  TUint16 BaseLow;
  TUint8 BaseMid;
  TUint8 Access;
  TUint8 Flags;
  TUint8 BaseHigh;
  TUint32 BaseUpper;
  TUint32 Reserved;
} PACKED;

struct TssDescriptor {
  TUint32 Reserved0;
  TUint64 StackTable[3];
  TUint64 Reserved1;
  TUint64 InterruptTable[7];
  TUint64 Reserved2;
  TUint16 Resered3;
  TUint16 IoMapBase;
  TUint8 IoMap[GDT_IOMAP_SIZE];
} PACKED;

static GdtDescriptor Descriptors[GDT_MAX_DESCRIPTORS] = { { 0 } };
GdtObject __GdtTableObject;
static TssDescriptor TssPointers[GDT_MAX_TSS] = { 0 };
static TssDescriptor BootTss = { 0 };

static int GdtInstallDescriptor(TUint64 Base, TUint32 Limit, TUint8 Access, TUint8 Granularity) {
  static int index = 0;

  int GdtIndex = index++;

  // Fill descriptor
  Descriptors[GdtIndex].BaseLow = (TUint16)(Base & 0xFFFF);
  Descriptors[GdtIndex].BaseMid = (TUint8)((Base >> 16) & 0xFF);
  Descriptors[GdtIndex].BaseHigh = (TUint8)((Base >> 24) & 0xFF);
  Descriptors[GdtIndex].BaseUpper = (TUint32)((Base >> 32) & 0xFFFFFFFF);
  Descriptors[GdtIndex].LimitLow = (TUint16)(Limit & 0xFFFF);
  Descriptors[GdtIndex].Flags = (TUint8)((Limit >> 16) & 0x0F);
  Descriptors[GdtIndex].Flags |= (Granularity & 0xF0);
  Descriptors[GdtIndex].Access = Access;
  Descriptors[GdtIndex].Reserved = 0;
  return GdtIndex;
}

GDT::GDT() {
  // Setup gdt-table object
  __GdtTableObject.Limit = (sizeof(GdtDescriptor) * GDT_MAX_DESCRIPTORS) - 1;
  __GdtTableObject.Base = (TUint64)&Descriptors[0];

  // Install NULL descriptor
  GdtInstallDescriptor(0, 0, 0, 0);

  // Kernel segments
  // Kernel segments span the entire virtual
  // address space from 0 -> 0xFFFFFFFF
  GdtInstallDescriptor(0, 0, GDT_RING0_CODE, GDT_GRANULARITY);
  GdtInstallDescriptor(0, 0, GDT_RING0_DATA, GDT_GRANULARITY);

  // Applications segments
  // Application segments does not span entire address space
  // but rather in their own subset
  GdtInstallDescriptor(0, 0, GDT_RING3_CODE, GDT_GRANULARITY);
  GdtInstallDescriptor(0, 0, GDT_RING3_DATA, GDT_GRANULARITY);

  // Shared segments
  // Extra segment shared between drivers and applications
  // which goes into the highest page-table
  // GdtInstallDescriptor(MEMORY_SEGMENT_EXTRA_BASE, (MEMORY_SEGMENT_EXTRA_SIZE - 1) / PAGE_SIZE, GDT_RING3_DATA, GDT_GRANULARITY);

  // Prepare gdt and tss for boot cpu
  gdt_flush(&__GdtTableObject);
  // GdtInstall();
  // TssInitialize(1);
}

#endif

#if 0
typedef struct _GDT_t_ {
  TUint16 segm_limit0;    /* segment limit, bits: 15:00	(00-15) */
  TUint16 base_addr0;     /* starting address, bits: 15:00	(16-31) */
  TUint8 base_addr1;      /* starting address, bits: 23:16	(32-38) */
  TUint8 type : 4;        /* segment type			(39-42) */
  TUint8 S : 1;           /* type: 0-system, 1-code or data	(43-43) */
  TUint8 DPL : 2;         /* descriptor privilege level	(44-45) */
  TUint8 P : 1;           /* present (in memory)		(46-46) */
  TUint8 segm_limit1 : 4; /*segment limit, bits: 19:16	(47-50) */
  TUint8 AVL : 1;         /* "Available for use"		(51-51) */
  TUint8 L : 1;           /* 64-bit code?			(52-52) */
  TUint8 DB : 1;          /* 1 - 32 bit system, 0 - 16 bit	(53-53) */
  TUint8 G : 1;           /* granularity 0-1B, 1-4kB 	(54-54) */
  TUint8 base_addr2;      /* starting address, bits: 23:16	(55-63) */
} PACKED GDT_t;

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

struct gdtr {
  TUint16 limit;
  TUint64 base;
  void Dump() {
    dprint("gdtr @ %x (%d bytes long)\n", this, sizeof(gdtr));
    dprint("limit  %x\n", limit);
    dprint("base  %x\n", base);
  }
} PACKED;

struct tss_info {
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
} PACKED;

static GDT_t gdtmem[6];
static gdtr gp;
static tss_info tss;

#define SEGNDX_CODE 1
#define SEGNDX_DATA 2
#define SEGNDX_TSS 3

#define PRIV_KERNEL 0
#define PRIV_USER 3


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
