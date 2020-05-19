target remote localhost:1234
file kernel/kernel.elf
set disassembly-flavor intel
set architecture i8086
layout asm
layout regs
display/i $pc
c

