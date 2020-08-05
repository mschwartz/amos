#  Booting

The very first instructions AMOS runs are its boot loader, so this is the place to start learning about AMOS.

AMOS currently supports LEGACY boot mode (not UEFI yet).  In Legacy boot mode, the BIOS loads one sector (sector #0) into RAM at 0x7c00 and looks at the last 2 bytes for 0x55aa.  If not 0x55aa, it's not a valid boot sector and nothing happens.  If it is 0x55aa, the BIOS jumps to 0x7c00, the first instruction of our boot loader.

Currently, assembly language for AMOS is written for the NASM assembler.  The boot program can be found in boot/boot.asm.

The file boot.asm consists of the instructions and data for the 512 byte boot sector AND as much additional code as we need to load our kernel and get the system into a proper state to jump to our kernel.  While we assemble our boot program (boot.asm) into a bigger binary than 512 bytes, the BIOS still only loads the first sector/512 bytes of our boot.bin.  What our first 512 bytes/boot program does is load in the rest of our boot.bin at 0x7c00 + 512 and then jumps to that.

* The memory map we use during boot (and later) is defined in boot/memory.inc.
* There are 16-bit mode debugging helper methods in boot/debug16.inc.
* There are 32 bit mode debugging helper methods in boot/debug32.inc.
* There are 64 bit mode debugging helper methods in boot/debug64.inc.

## 16 bit mode

In Leagacy boot, the CPU is initially in 16 bit mode (Legacy 8086 mode!), with segment registers, 640K, and all that ugliness.

Our objective is to get the CPU into 64 bit mode.

The first instruction is a jmp 0:main, which branches over some variables and assures CS is 0.  There are 5x 16-bit words immediately following the jmp instruction:

1) boot_sector is the LBA (Logical Block Address) of the rest of the boot program.
2) boot_sectors is the number of sectors that make up the rest of the boot program.
3) kernel_sector is the LBA of our kernel.
4) kernel_sectors is the number of sectors that make up the kernel.
5) root_sector is the LBA of our file system.

The tools/build-img tool reads in our boot sector, our kernel, and a file system image and writes out a binary image.  The output file has these 5 16-bit words patched with the right avalues.  build-img knows the size of each of the boot program, kernel, and file system.

At main, we set up the segment registers.  

The BIOS preloads the dl register with the BIOS boot drive number, so we save that for later use.  Since we're using BIOS calls to read from the boot drive, we don't have to care about whether the boot drive is a USB CDROM/thumb drive, floppy disk, or ATA hard drive.

The program in the first 512 bytes does (google to learn about each step):

1) Detect running in bochs
2) Initialize serial port debugging
3) Enable A20
4) Load the rest of the boot program
5) jump to start of the rest of the boot program

The 512 byte boot sector doesn't allow for enough space for the code we need to get the system into 64 bit mode.  But since we've loaded the rest of our boot program/additional sectors, we have all the space we need for the code we need.  We use a lot of the 512 bytes for the debug16.inc routines!

The next steps are:

1) We copy the EBDA (Extended BIOS Data Area) out of the way - we need some contiguous RAM to load in the kernel.  
2) We load in the kernel at 0x40000, for as many sectors as needed.  There is a limit, though, as 0xa0000 is video RAM.
3) We use BIOS to identify the VESA graphics modes supported by the machine.  We have a struct for the current mode and an array of structs following for all the modes.  This RAM is available for the kernel to access the modes.
4) We use BIOS to identify the memory regions available.  RAM is not all contiguous.  There is at least RAM below the VGA RAM at 0xa0000 and RAM above the text mode VGA RAM.
5) We then put the CPU into 32 bit mode.  This requires a 32-bit GDT and setting a bit in cr0.  Once in 32-bit mode, the BIOS methods are no longer available to be called.

We use 32-bit mode to set things up to get into 64 bit (long) mode.  It's possible to go from 16-bit mode to 64-bit mode directly, but the Intel/AMD manuals say to do it in steps.

In 32-bit mode, we can copy the kernel we loaded to its ultimate location at/above 1MB (0x100000).  We have to set up a 64-bit GDT and set up paging (a 64-bit GDT and paging setup is required for 64-bit mode).

The pure64 boot loader (on GitHub) has a routine to identity map the first 64G (for paging).  When we enter our kernel, we have address space from 0-64G.

Once in 64-bit mode, we set up a SYSINFO struct with values we gathered during boot.  The address of this SYSINFO is passed to kernel_start.

## Next steps
kernel_start.asm is part of the kernel itself, not part of the boot program.  It sets up the C/C++ environment and calls kernel_main().


