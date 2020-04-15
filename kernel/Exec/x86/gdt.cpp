#include <gdt.h>
#include <kprint.h>
#include <bochs.h>

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
    kprint("gdt_info @ %x (%d bytes long)\n", this, sizeof(gdt_info));
    kprint("limit low %x ", limit_low);
    kprint("base low %x ", base_low);
    kprint("base middle %x ", base_middle);
    kprint("access %x ", access);
    kprint("granularity %x ", granularity);
    kprint("base high %x\n", base_high);
  }
} PACKED;

struct gdtr {
  TUint16 limit;
  TUint64 base;
  void Dump() {
    kprint("gdtr @ %x (%d bytes long)\n", this, sizeof(gdtr));
    kprint("limit  %x\n", limit);
    kprint("base  %x\n", base);
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

extern "C" void gdt_flush(gdtr *gp);         // ASM function
extern "C" void tss_flush(TUint32 segment); // ASM funct

GDT::GDT() {
  return;
//  kprint("sizeof(GDT_t) = %d\n", sizeof(GDT_t));
//  dprintf("sizeof(GDT_t) = %d\n", sizeof(GDT_t));
//  dprint("sizeof(GDT_t) = %d %x\n", sizeof(GDT_t), 0xdeadbeef);
  //  return;
  gp.limit = (sizeof(gdt_info) * 6) - 1;
  gp.base = (TUint64)&gdtmem[0];
//  gp.Dump();
  set_gate(0, 0, 0, 0, 0);                                   // first gate is ALWAYS null
  set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xC0);                    // 0x9A corresponds to ring 0 CODE memory segments
  set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xC0);                    // 0x92 corresponds to  ring 0 DATA memory segments
  set_gate(3, 0, 0xFFFFFFFF, 0xF8, 0xC0);                    // User Code (r3) segment maybe these 2 shouldn't overlap with the ring 0...
  set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xC0);                    // User Data (r3) segment
  set_gate(5, (TUint64)&tss, sizeof(tss_info), 0x89, 0x40); // TSS segment

//  kprint("about to flush %x\n", &gp);
  gdt_flush(&gp);
//  tss_install();
}

GDT::~GDT() {
  //
}

void GDT::set_gate(TInt id, void *start_addr, TUint32 size, TUint32 priv_level) {
  TUint64 addr = (TUint64)start_addr;
  TUint32 gsize = size;
  kprint("gsize: %x\n", gsize);
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
