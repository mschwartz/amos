#include "cpu.h"
#include "idt.h"
#include <Exec/ExecBase.h>

/*!
 * CPU class maintains the CPU interrupt vectors
 */

static TBool divide_error_handler(void *aData) {
  dprint("divide error handler\n");
  return true;
}
static TBool debug_handler(void *aData) {
  dprint("debug handler\n");
  return true;
}
static TBool nmi_handler(void *aData) {
  dprint("nmi handler\n");
  return true;
}
static TBool breakpoint_handler(void *aData) {
  dprint("breakpoint handler\n");
  return true;
}
static TBool overflow_handler(void *aData) {
  dprint("overflow handler\n");
  return true;
}
static TBool out_of_bounds_handler(void *aData) {
  dprint("out_of_bounds handler\n");
  return true;
}
static TBool invalid_opcode_handler(void *aData) {
  cli();
  dprint("invalid_opcode handler\n");
  ExecBase::DumpCurrentTaskRegisters();
  halt();
  return true;
}
static TBool device_not_available_handler(void *aData) {
  dprint("device_not_available handler\n");
  return true;
}
static TBool double_fault_handler(void *aData) {
  dprint("double_fault handler\n");
  return true;
}
static TBool coprocessor_segment_overrun_handler(void *aData) {
  dprint("coprocessor_segment_overrun handler\n");
  return true;
}
static TBool invalid_tss_handler(void *aData) {
  dprint("invalid_tss handler\n");
  return true;
}
static TBool segment_not_present_handler(void *aData) {
  dprint("segment_not_present handler\n");
  return true;
}
static TBool stack_fault_handler(void *aData) {
  dprint("stack_fault handler\n");
  return true;
}
static TBool general_protection_handler(void *aData) {
  dprint("general_protection handler\n");
  halt();
  return true;
}
static TBool page_fault_handler(void *aData) {
  dprint("page_fault handler\n");
  return true;
}
static TBool unknown_exception_handler(void *aData) {
  dprint("unknown_exception handler\n");
  return true;
}
static TBool coprocessor_fault_handler(void *aData) {
  dprint("coprocessor_fault handler\n");
  return true;
}
static TBool alignment_check_handler(void *aData) {
  dprint("alignment_check handler\n");
  return true;
}
static TBool machine_check_handler(void *aData) {
  dprint("machine_check handler\n");
  return true;
}
static TBool simd_fault_handler(void *aData) {
  dprint("simd_fault handler\n");
  return true;
}
static TBool virtualization_handler(void *aData) {
  dprint("virtualization_handler\n");
  return true;
}
static TBool security_handler(void *aData) {
  dprint("security_handler\n");
  return true;
}

CPU *gCPU;

CPU::CPU() {
  TInt irq = 0;
  IDT::install_handler(irq++, divide_error_handler, /* data */ nullptr, "Divide Error");
  IDT::install_handler(irq++, debug_handler, /* data */ nullptr, "Debug Exception");
  IDT::install_handler(irq++, nmi_handler, /* data */ nullptr, "NMI");
  IDT::install_handler(irq++, breakpoint_handler, /* data */ nullptr, "INT 3/Breakpoint");
  IDT::install_handler(irq++, overflow_handler, /* data */ nullptr, "Overflow/INTO Exception");
  IDT::install_handler(irq++, out_of_bounds_handler, /* data */ nullptr, "Bounds Check/BOUND");
  IDT::install_handler(irq++, invalid_opcode_handler, /* data */ nullptr, "Invlaid Opcode");
  IDT::install_handler(irq++, device_not_available_handler, /* data */ nullptr, "Coprocessor Not Availale");
  IDT::install_handler(irq++, double_fault_handler, /* data */ nullptr, "Double Fault Exception");
  IDT::install_handler(irq++, coprocessor_segment_overrun_handler, /* data */ nullptr, "Coprocessor Segment Overrun");
  IDT::install_handler(irq++, invalid_tss_handler, /* data */ nullptr, "Invalid TSS");
  IDT::install_handler(irq++, segment_not_present_handler, /* data */ nullptr, "Segment Not Present");
  IDT::install_handler(irq++, stack_fault_handler, /* data */ nullptr, "Stack Exception");
  IDT::install_handler(irq++, general_protection_handler, /* data */ nullptr, "General Protection Exception");
  IDT::install_handler(irq++, page_fault_handler,  /* data */ nullptr, "Page Fault Exception");
  IDT::install_handler(irq++, unknown_exception_handler, /* data */ nullptr, "Intel Reserved");
  IDT::install_handler(irq++, coprocessor_fault_handler, /* data */ nullptr, "Coprocessor Error");
  IDT::install_handler(irq++, alignment_check_handler, /* data */ nullptr, "Alignment Check");
  IDT::install_handler(irq++, machine_check_handler, /* data */ nullptr, "Machine Check Exception");
  IDT::install_handler(irq++, simd_fault_handler, /* data */ nullptr, "SIMD Fault Exception");
  IDT::install_handler(irq++, virtualization_handler, /* data */ nullptr, "Virtualization Exception");
  IDT::install_handler(irq++, security_handler, /* data */ nullptr, "Security Exception");
}
