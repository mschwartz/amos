#include <Exec/x86/cpu.h>
#include <Exec/x86/tss.h>
#include <Exec/x86/gdt.h>
#include <Exec/Memory.h>

/*
GDT Entry:

 3              1 1
 1              6 5              0
+----------------+----------------+
| Base 0:15      | Limit 0:15     |
+----------------+----------------+

 6      5 5  5 5    4 4     4 3       3
 3      6 5  2 1    8 7     0 9       2
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

extern "C" void gdt_flush(TAny *gp, TUint16 cs);        // ASM function
extern "C" void tss_flush(TUint32 segment); // ASM funct

struct gdtp {
  TUint16 len;
  TUint64 *gdt;
} PACKED;

const int SEG_NULL = 0;
const int SEG_KCODE = 1;
const int SEG_KDATA = 2;
const int SEG_UCODE = 3;
const int SEG_UDATA = 4;
const int SEG_TSS = 5;
const int SEG_TSS_HIGH = 6;

#define GDT_CODE (0x18L << 8)
#define GDT_DATA (0x12L << 8)
#define GDT_TSS (0x89L << 8)
#define GDT_DPL(lvl) ((lvl) << 13)
#define GDT_PRESENT (1L << 15)
#define GDT_LONG (1L << 21)

TUint64 gGdt[8] ALIGN16;
struct gdtp gGdtp = { sizeof(gGdt), gGdt };

#define KSTACK_SIZE (2 * 1024 * 1024)

TUint8 gKStack1[KSTACK_SIZE] ALIGN16;

static void PrintSelector(TUint64 aSelector) {
  dprint("Selector %016x\n", aSelector);
  dprint("Access: %x\n", (aSelector >> 40) & 0xff);
  dprint("\n");
}

GDT::GDT(TSS *aTss) {
  // kernel segments
  gGdt[SEG_NULL] = 0;
  gGdt[SEG_KCODE] = (GDT_PRESENT | GDT_DPL(0) | GDT_CODE | GDT_LONG) << 32; // 8
  gGdt[SEG_KDATA] = (GDT_PRESENT | GDT_DPL(0) | GDT_DATA) << 32;            // 0x10
  gGdt[SEG_UCODE] = (GDT_PRESENT | GDT_DPL(3) | GDT_CODE | GDT_LONG) << 32; // 0x18
  gGdt[SEG_UDATA] = (GDT_PRESENT | GDT_DPL(3) | GDT_DATA) << 32;            // 0x20

  // TSS
  struct tss *tss = (struct tss *)&aTss->mTss;
  // SetMemory8(tss, 0, sizeof(struct tss));
  // tss->io_mba = sizeof(struct tss);
  // tss->rsp0 = (TUint64)&gKStack1[KSTACK_SIZE];
  // tss->Dump();
  // dlog("tss rsp0: %016x\n", tss->rsp0);

  TUint64 tss_limit = sizeof(struct tss);
  TUint64 tss_base = (TUint64)tss;

  // TUint64 addr = tss_base;
  gGdt[SEG_TSS] = (0x0067) | ((tss_base & 0xFFFFFF) << 16) | (0x00E9LL << 40) |
                 (((tss_base >> 24) & 0xFF) << 56);
  gGdt[SEG_TSS_HIGH] = (tss_limit >> 32L);

  gGdtp.len = 7 * 8 - 1;
  gGdtp.gdt = gGdt;

  gdt_flush(&gGdtp, SEG_KDATA << 3); //0x10);
  tss_flush(SEG_TSS << 3);
  // tss_flush(0x28);
}
