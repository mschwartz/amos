# MEMORY MAP

Memory usage depends on what stage of boot is going on and then when the kernel is running in 64 bit mode.

### PHYSICAL MEMORY

This information is potentially out of date.  See the memory.inc file for accurate values

0x5000: (max 4096 bytes) global system information struct - see TSystemInfo struct in ExecBase.h
0x6000: miscellaneous buffer
0x7b00: This is the top of stack during boot stages
0x7c00: The boot program (boot sector plus boot_sectors sectors)
0xa000: This is information about the video modes selected and available
0xb000: This is an array of memory area information structs

0x10000: MMU Paging Tables (through 0x5ffff)
0x60000: This is where the kernel is loaded using BIOS int 13h.  It is later moved to KERNEL_ORG.
0x80000-0x9ffff: this is the EBDA
0x200000: This is where the kernel is copied and runs
