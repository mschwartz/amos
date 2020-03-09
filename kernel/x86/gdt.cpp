#include <gdt.h>
#include <kprint.h>
#include <bochs.h>

GDT *gdt;

typedef struct _GDT_t_ {
  uint16_t segm_limit0;    /* segment limit, bits: 15:00	(00-15) */
  uint16_t base_addr0;     /* starting address, bits: 15:00	(16-31) */
  uint8_t base_addr1;      /* starting address, bits: 23:16	(32-38) */
  uint8_t type : 4;        /* segment type			(39-42) */
  uint8_t S : 1;           /* type: 0-system, 1-code or data	(43-43) */
  uint8_t DPL : 2;         /* descriptor privilege level	(44-45) */
  uint8_t P : 1;           /* present (in memory)		(46-46) */
  uint8_t segm_limit1 : 4; /*segment limit, bits: 19:16	(47-50) */
  uint8_t AVL : 1;         /* "Available for use"		(51-51) */
  uint8_t L : 1;           /* 64-bit code?			(52-52) */
  uint8_t DB : 1;          /* 1 - 32 bit system, 0 - 16 bit	(53-53) */
  uint8_t G : 1;           /* granularity 0-1B, 1-4kB 	(54-54) */
  uint8_t base_addr2;      /* starting address, bits: 23:16	(55-63) */
} PACKED GDT_t;

struct gdt_info {
  unsigned short limit_low;
  unsigned short base_low;
  unsigned char base_middle;
  unsigned char access;
  unsigned char granularity;
  unsigned char base_high;
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
  uint16_t limit;
  uint64_t base;
  void Dump() {
    kprint("gdtr @ %x (%d bytes long)\n", this, sizeof(gdtr));
    kprint("limit  %x\n", limit);
    kprint("base  %x\n", base);
  }
} PACKED;

struct tss_info {
  uint32_t rsvd0;
  uint64_t rsp0;
  uint64_t rsp1;
  uint64_t rsp2;
  uint32_t rsvd1;
  uint32_t rsvd2;
  uint64_t ist1;
  uint64_t ist2;
  uint64_t ist3;
  uint64_t ist4;
  uint64_t ist5;
  uint64_t ist6;
  uint64_t ist7;
  uint32_t rsvd3;
  uint32_t rsvd4;
  uint16_t rsvd5;
  uint16_t iopb;
} PACKED;

GDT_t gdtmem[6];
gdtr gp;
tss_info tss;

#define SEGNDX_CODE 1
#define SEGNDX_DATA 2
#define SEGNDX_TSS 3

#define PRIV_KERNEL 0
#define PRIV_USER 3

extern "C" void gdt_flush(gdtr *gp);         // ASM function
extern "C" void tss_flush(uint32_t segment); // ASM funct

GDT::GDT() {
  return;
//  kprint("sizeof(GDT_t) = %d\n", sizeof(GDT_t));
//  dprintf("sizeof(GDT_t) = %d\n", sizeof(GDT_t));
//  dprint("sizeof(GDT_t) = %d %x\n", sizeof(GDT_t), 0xdeadbeef);
  //  return;
  gp.limit = (sizeof(gdt_info) * 6) - 1;
  gp.base = (uint64_t)&gdtmem[0];
//  gp.Dump();
  set_gate(0, 0, 0, 0, 0);                                   // first gate is ALWAYS null
  set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xC0);                    // 0x9A corresponds to ring 0 CODE memory segments
  set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xC0);                    // 0x92 corresponds to  ring 0 DATA memory segments
  set_gate(3, 0, 0xFFFFFFFF, 0xF8, 0xC0);                    // User Code (r3) segment maybe these 2 shouldn't overlap with the ring 0...
  set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xC0);                    // User Data (r3) segment
  set_gate(5, (uint64_t)&tss, sizeof(tss_info), 0x89, 0x40); // TSS segment

//  kprint("about to flush %x\n", &gp);
  gdt_flush(&gp);
//  tss_install();
}

GDT::~GDT() {
  //
}

void GDT::set_gate(int id, void *start_addr, uint32_t size, uint32_t priv_level) {
  uint64_t addr = (uint64_t)start_addr;
  uint32_t gsize = size;
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

void GDT::set_gate(int id, uint64_t base, uint64_t limit, uint8_t access, uint8_t granularity) {
  GDT_t *g = (GDT_t *)&gdtmem[id];
  uint32_t addr = (uint32_t)base;
  uint32_t gsize = limit;

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
