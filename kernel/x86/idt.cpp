#include <x86/idt.h>
#include <posix/string.h>
#include <x86/kprint.h>
#include <x86/bochs.h>
#include <x86/tasking.h>
#include <x86/kernel_memory.h>

IDT *gIDT;

extern "C" void set_vector(void *idt_vector, void (*offset)(), TUint16 selector, uint8_t flags);
extern "C" void load_idtr(void *ptr);

// exceptions
extern "C" uint64_t isr0;
extern "C" uint64_t isr1;
extern "C" uint64_t isr2;
extern "C" uint64_t isr3;
extern "C" uint64_t isr4;
extern "C" uint64_t isr5;
extern "C" uint64_t isr6;
extern "C" uint64_t isr7;
extern "C" uint64_t isr8;
extern "C" uint64_t isr9;
extern "C" uint64_t isr10;
extern "C" uint64_t isr11;
extern "C" uint64_t isr12;
extern "C" uint64_t isr13;
extern "C" uint64_t isr14;
extern "C" uint64_t isr15;
extern "C" uint64_t isr16;
extern "C" uint64_t isr17;
extern "C" uint64_t isr18;
extern "C" uint64_t isr19;
extern "C" uint64_t isr20;
extern "C" uint64_t isr21;
extern "C" uint64_t isr22;
extern "C" uint64_t isr23;
extern "C" uint64_t isr24;
extern "C" uint64_t isr25;
extern "C" uint64_t isr26;
extern "C" uint64_t isr27;
extern "C" uint64_t isr28;
extern "C" uint64_t isr29;
extern "C" uint64_t isr30;
extern "C" uint64_t isr31;

// IRQs
extern "C" uint64_t isr32; // the timer irq (0)
extern "C" uint64_t isr33; // the kb irq (1)
extern "C" uint64_t isr34;
extern "C" uint64_t isr35;
extern "C" uint64_t isr36;
extern "C" uint64_t isr37;
extern "C" uint64_t isr38;
extern "C" uint64_t isr39;
extern "C" uint64_t isr40;
extern "C" uint64_t isr41;
extern "C" uint64_t isr42;
extern "C" uint64_t isr43;
extern "C" uint64_t isr44;
extern "C" uint64_t isr45;
extern "C" uint64_t isr46; // ata 0,0 (14)
extern "C" uint64_t isr47; // ata 0,1 (15)

extern "C" uint64_t isr48; // unexpected

extern "C" uint64_t isr128; // syscall (0x80, or 0x60 + 32)
extern "C" uint64_t isr130; // interrupt scheduler

static isr_handler_t interrupt_handlers[INTERRUPTS];

extern "C" bool kernel_isr() {
//  dprint("kernel_isr\n");
//  current_task->Dump();
  isr_handler_t *info = &interrupt_handlers[current_task->isr_num];
  if (!info->handler) {
    const char *desc = IDT::interrupt_description(current_task->isr_num);
    dprint("here %s\n", desc);
    kprint("here %s\n", desc);
    kprint("no handler: %s\n", desc);
    return false;
  }
  return info->handler(info->data);
};

#define IDT_PRESENT    ((uint64_t)1 << 47)
#define IDT_64INT      ((uint64_t)14 << 40)
#define IDT_64TRAP     ((uint64_t)15 << 40)
#define IDT_USER       ((uint64_t)3 << 45)
#define IDT_SYSCALL    0x80
#define IDT_SIZE       (IDT_SYSCALL + 1)
#define IDT_IRQS       16
#define IDT_EXCEPTIONS 32

typedef struct idt_entry {
  uint64_t low;
  uint64_t high;
  void set(uint64_t isr, uint64_t flags) {
    uint64_t cs = 8;
    low = (isr & 0xFFFFul) | ((uint64_t)cs << 16) | ((isr & 0xFFFF0000ul) << 32) | flags | IDT_PRESENT;
    high = (isr >> 32) & 0xFFFFFFFFul;
  }
} PACKED idt_entry_t;

typedef struct {
  TUint16 limit;
  idt_entry_t *base;
} PACKED idt_ptr_t;

static idt_entry_t idt_entries[IDT_SIZE];
static idt_ptr_t idt_ptr;

IDT::IDT() {
  disable_interrupts();
  // initialize C ISRs
  for (int i = 0; i < INTERRUPTS; i++) {
    interrupt_handlers[i].set(nullptr, nullptr, "Not installed");
  }

    dprint("isr0 %x\n", isr0);
    dprint("isr1 %x\n", isr1);
    dprint("isr2 %x\n", isr2);

  // EXCEPTIONS
  idt_entries[0].set(isr0, IDT_64INT);
  idt_entries[1].set(isr1, IDT_64INT);
  idt_entries[2].set(isr2, IDT_64INT);
  idt_entries[3].set(isr3, IDT_64INT);
  idt_entries[4].set(isr4, IDT_64INT);
  idt_entries[5].set(isr5, IDT_64INT);
  idt_entries[6].set(isr6, IDT_64INT);
  idt_entries[7].set(isr7, IDT_64INT);
  idt_entries[8].set(isr8, IDT_64INT);
  idt_entries[9].set(isr9, IDT_64INT);
  idt_entries[10].set(isr10, IDT_64INT);
  idt_entries[11].set(isr11, IDT_64INT);
  idt_entries[12].set(isr12, IDT_64INT);
  idt_entries[13].set(isr13, IDT_64INT);
  idt_entries[14].set(isr14, IDT_64INT);
  idt_entries[15].set(isr15, IDT_64INT);
  idt_entries[16].set(isr16, IDT_64INT);
  idt_entries[17].set(isr17, IDT_64INT);
  idt_entries[18].set(isr18, IDT_64INT);
  idt_entries[19].set(isr19, IDT_64INT);
  idt_entries[20].set(isr20, IDT_64INT);
  idt_entries[21].set(isr21, IDT_64INT);
  idt_entries[22].set(isr22, IDT_64INT);
  idt_entries[23].set(isr23, IDT_64INT);
  idt_entries[24].set(isr24, IDT_64INT);
  idt_entries[25].set(isr25, IDT_64INT);
  idt_entries[26].set(isr26, IDT_64INT);
  idt_entries[27].set(isr27, IDT_64INT);
  idt_entries[28].set(isr28, IDT_64INT);
  idt_entries[29].set(isr29, IDT_64INT);
  idt_entries[30].set(isr30, IDT_64INT);
  idt_entries[31].set(isr31, IDT_64INT);

  // IRQs
  idt_entries[32].set(isr32, IDT_64INT);
  idt_entries[33].set(isr33, IDT_64INT);
  idt_entries[34].set(isr34, IDT_64INT);
  idt_entries[35].set(isr35, IDT_64INT);
  idt_entries[36].set(isr36, IDT_64INT);
  idt_entries[37].set(isr37, IDT_64INT);
  idt_entries[38].set(isr38, IDT_64INT);
  idt_entries[39].set(isr39, IDT_64INT);
  idt_entries[40].set(isr40, IDT_64INT);
  idt_entries[41].set(isr41, IDT_64INT);
  idt_entries[42].set(isr42, IDT_64INT);
  idt_entries[43].set(isr43, IDT_64INT);
  idt_entries[44].set(isr44, IDT_64INT);
  idt_entries[45].set(isr45, IDT_64INT);
  idt_entries[46].set(isr46, IDT_64INT);
  idt_entries[47].set(isr47, IDT_64INT);

  // install handler for unexpected interrupts
  for (int i=48; i<IDT_SIZE; i++) {
    // TODO: make a special handler
    idt_entries[i].set(isr48, IDT_64INT);
  }
  idt_entries[128].set(isr128, IDT_64INT);
  idt_entries[130].set(isr130, IDT_64INT);

  idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
  idt_ptr.base = &idt_entries[0];
  load_idtr(&idt_ptr);
}

#if 0
struct idt_entry {
  TUint16 offset1;
  TUint16 selector;
  uint8_t ist;
  uint8_t type_attr;
  TUint16 offset2;
  uint32_t offset3;
  uint32_t zero;
  void set(uint64_t offset, TUint16 selector, uint8_t flags) {
    offset1 = offset & 0xffff;
    this->selector = selector;
    ist = 0;
    type_attr = flags & 0xFF;
    offset2 = (offset >> 16) & 0xffff;
    offset2 = (offset >> 32) & 0xffffffff;
    zero = 0;
  }
  void Dump() {
    dprint("idt_entry @ %x (size: %d)\n", this, sizeof(idt_entry));
    //    dprint("*** %d %d %d %d\n", sizeof(uint8_t), sizeof(TUint16), sizeof(uint32_t), sizeof(uint64_t));
    dprint("  selector: %x offset1: %x offset2: %x offset3: %x ist: %x attr: %x zero:%x\n", selector, offset1, offset2, offset3, ist, type_attr, zero);
    //    dprint("*** %x %x %x %x %x %x %x %x\n", this, &this->offset1, &this->selector, &this->ist, &this->type_attr, &this->offset2, &this->offset3, &this->zero);
  }
} PACKED;

typedef idt_entry idt_entry_t;

static idt_entry_t idt_entries[256];

struct {
  TUint16 limit;
  void *base;
} PACKED idt_p;

extern "C" bool kernel_isr() {
//  dprint("kernel_isr\n");
//  current_task->Dump();
  HANDLER_INFO *info = &interrupt_handlers[current_task->isr_num];
  if (!info->handler) {
    const char *desc = IDT::interrupt_description(current_task->isr_num);
    dprint("here %s\n", desc);
    kprint("here %s\n", desc);
    kprint("no handler: %s\n", desc);
    return false;
  }
  return info->handler(info->data);
};

static void idt_set_gate(int i, uint64_t offset, TUint16 selector, uint8_t flags) {
  //    dprint("idt_set_gate ndx: %d %x\n", i, offset&0xfffffff);
  idt_entries[i].set(offset, selector, flags);
}

IDT::IDT() {
  disable_interrupts();
  memset((void *)&idt_entries, 0x0, sizeof(struct idt_entry) * 256);

  // initialize C ISRs
  for (int i = 0; i < INTERRUPTS; i++) {
    interrupt_handlers[i].set(nullptr, nullptr, "Not installed");
  }

    dprint("isr0 %x\n", isr0);
    dprint("isr1 %x\n", isr1);
    dprint("isr2 %x\n", isr2);

  // Exceptions
  idt_set_gate(0, isr0, 0x08, 0x8E);
  idt_set_gate(1, isr1, 0x08, 0x8E);
  idt_set_gate(2, isr2, 0x08, 0x8E);
  idt_set_gate(3, isr3, 0x08, 0x8E);
  idt_set_gate(4, isr4, 0x08, 0x8E);
  idt_set_gate(5, isr5, 0x08, 0x8E);
  idt_set_gate(6, isr6, 0x08, 0x8E);
  idt_set_gate(7, isr7, 0x08, 0x8E);
  idt_set_gate(8, isr8, 0x08, 0x8E);
  idt_set_gate(9, isr9, 0x08, 0x8E);
  idt_set_gate(10, isr10, 0x08, 0x8E);
  idt_set_gate(11, isr11, 0x08, 0x8E);
  idt_set_gate(12, isr12, 0x08, 0x8E);
  idt_set_gate(13, isr13, 0x08, 0x8E);
  idt_set_gate(14, isr14, 0x08, 0x8E);
  idt_set_gate(15, isr15, 0x08, 0x8E);
  idt_set_gate(16, isr16, 0x08, 0x8E);
  idt_set_gate(17, isr17, 0x08, 0x8E);
  idt_set_gate(18, isr18, 0x08, 0x8E);
  idt_set_gate(19, isr19, 0x08, 0x8E);
  idt_set_gate(20, isr20, 0x08, 0x8E);
  idt_set_gate(21, isr21, 0x08, 0x8E);
  idt_set_gate(22, isr22, 0x08, 0x8E);
  idt_set_gate(23, isr23, 0x08, 0x8E);
  idt_set_gate(24, isr24, 0x08, 0x8E);
  idt_set_gate(25, isr25, 0x08, 0x8E);
  idt_set_gate(26, isr26, 0x08, 0x8E);
  idt_set_gate(27, isr27, 0x08, 0x8E);
  idt_set_gate(28, isr28, 0x08, 0x8E);
  idt_set_gate(29, isr29, 0x08, 0x8E);
  idt_set_gate(30, isr30, 0x08, 0x8E);
  idt_set_gate(31, isr31, 0x08, 0x8E);

  // IRQs
  idt_set_gate(32, isr32, 0x08, 0x8E);
  idt_set_gate(33, isr33, 0x08, 0x8E);
  idt_set_gate(34, isr34, 0x08, 0x8E);
  idt_set_gate(35, isr35, 0x08, 0x8E);
  idt_set_gate(36, isr36, 0x08, 0x8E);
  idt_set_gate(37, isr37, 0x08, 0x8E);
  idt_set_gate(38, isr38, 0x08, 0x8E);
  idt_set_gate(39, isr39, 0x08, 0x8E);
  idt_set_gate(40, isr40, 0x08, 0x8E);
  idt_set_gate(41, isr41, 0x08, 0x8E);
  idt_set_gate(43, isr42, 0x08, 0x8E);
  idt_set_gate(44, isr44, 0x08, 0x8E);
  idt_set_gate(45, isr45, 0x08, 0x8E);
  idt_set_gate(46, isr46, 0x08, 0x8E);
  idt_set_gate(47, isr47, 0x08, 0x8E);

  for (int i = 49; i <= 255; i++) {
    // FIXME: create a special dummy handler
    idt_set_gate(i, isr0, 0x08, 0x8E);
  }

  idt_set_gate(0x80, isr128, 0x08, 0x8E);
  //  idt_entries[0x80].Dump();
  idt_set_gate(0x82, isr130, 0x08, 0x8E); // SCHEDULE INTERRUPT

  idt_p.limit = sizeof(struct idt_entry) * 256 - 1;
  idt_p.base = &idt_entries[0];
  load_idtr(&idt_p);
}
#endif
IDT::~IDT() {
  disable_interrupts();
}

void IDT::install_handler(uint8_t index, TInterruptHandler *handler, void *aData, const char *description) {
  interrupt_handlers[index].set(handler, aData, description);
}

static const char *int_desc[] = {
  /* "exceptions" - interrupts generated in processor */
  /* 00 */ "Divide Error",
  /* 01 */ "Debug",
  /* 02 */ "Non Maskable Interrupt",
  /* 03 */ "Breakpoint",
  /* 04 */ "Overflow",
  /* 05 */ "Out of Bounds",
  /* 06 */ "Invalid Opcode",
  /* 07 */ "Device Not Available",
  /* 08 */ "Double Fault",
  /* 09 */ "Coprocessor Segment Overrun",
  /* 10 */ "Invalid TSS",
  /* 11 */ "Segment Not Present",
  /* 12 */ "Stack Fault",
  /* 13 */ "General Protection",
  /* 14 */ "Page Fault",
  /* 15 */ "Unknown Exception",
  /* 16 */ "Coprocessor Fault",
  /* 17 */ "Alignment Check",
  /* 18 */ "Machine Check",
  /* 19 */ "SIMD Fault",

  /* Intel reserved interrupts */
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",
  "Intel reserved",

  /* External interrupts (generated outside processor) */
  "IRQ_TIMER",
  "IRQ_KEYBOARD",
  "IRQ_SLAVE_PIC",
  "IRQ_COM2",
  "IRQ_COM1",
  "IRQ_LPT2",
  "IRQ_FLOPPY",
  "IRQ_LPT1",
  "IRQ_RT_CLOCK",
  "IRQ_MASTER_PIC",
  "IRQ_RESERVED1",
  "IRQ_RESERVED2",
  "IRQ_RESERVED3",
  "IRQ_COPROCESSOR",
  "IRQ_HARD_DISK",
  "IRQ_RESERVED4",

  /* Software interrupt, using first available = 48 */
  "Software interrupt"
};

const char *IDT::interrupt_description(TUint16 n) {
  /* Interrupts descriptions */
  dprint("desc(%d) %s\n", n, int_desc[0]);

  if (n < INTERRUPTS)
    return int_desc[n];
  else
    return "Unknown interrupt number";
}
