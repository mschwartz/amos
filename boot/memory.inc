;; memory.inc
	; memory map

gigabyte:            equ 0x40000000

misc_buffer:         equ 0x6000          ; 512 byte buffer for misc. use

sys_info:            equ 0x5000
BOOTSTRAP_ORG:       equ 0x7c00

	; kernel address (loaded here)
KERNEL_ORG:          equ 0x100000
KERNEL_LOAD:         equ 0x60000
KERNEL_LOAD_SEG:     equ 0x6000

	; display
CGA:                 equ 0xb8000

	; STACK POINTER in 16, 32, 64 bit modes
SP16:                equ 0x7c00
ESP32:               equ 0x7c00
RSP64:               equ 0x7c00

video_info:          equ 0xa000
memory_info:         equ 0xb000

	; page tables
PLL4T:               equ 0xc000          ; page map level 4 table
PDPT:                equ 0xd000          ; page directory pointer table
PDT:                 equ 0xe000          ; page directory table
PT:                  equ 0xf000          ; page table

;;
;; we are going to create one of these, fill it in, and pass it to kernel_main/kernel_start
;; we locate it at 5000h, which will be out of the way when we're all set up and ready to jump to kernel
;;
struc SYSINFO
.milliseconds resq 1
.video_mode resq 1
.framebuffer resq 1
.screen_width resq 1
.screen_height resq 1
.screen_depth resq 1
.screen_pitch resq 1
.ram resq 1
.boot_sector resq 1
.boot_sectors resq 1
.kernel_sector resq 1
.kernel_sectors resq 1
.root_sector resq 1

.boot_drive resq 1
.num_drives resq 1
.num_heads resq 1
.num_cylinders resq 1
.sectors_per_track resq 1
.disk_cx resq 1

.cpu_mhz resq 1
.cpu_cores resq 1
.bochs resq 1
.init_start resq 1
.init_end resq 1
.text_start resq 1
.text_end resq 1
.rodata_start resq 1
.rodata_end resq 1
.data_start resq 1
.data_end resq 1
.bss_start resq 1
.bss_end resq 1
.kernel_end resq 1
.disk_size resq 1
.version resb 64	
endstruc

