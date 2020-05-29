target remote localhost:1234
file kernel/kernel.elf
set disassembly-flavor intel
set architecture auto
show architecture
br *0x7c00
#layout asm
#layout regs
display/i $pc
#hbreak *0x7c00
#c

