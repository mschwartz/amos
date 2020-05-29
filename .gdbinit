target remote localhost:1234
set disassembly-flavor intel
set architecture i8086
br *0x7c00
layout asm
layout regs
display/i $pc
c

