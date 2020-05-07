;; memory.inc
; memory map

gigabyte            equ 0x40000000

misc_buffer         equ 0x6000          ; 512 byte buffer for misc. use

BOOTSTRAP_ORG       equ 0x7c00

; kernel address (loaded here)
KERNEL_ORG          equ 0x100000
KERNEL_LOAD         equ 0x60000
KERNEL_LOAD_SEG     equ 0x6000

; display
CGA                 equ 0xb8000

; STACK POINTER in 16, 32, 64 bit modes
SP16                equ 0x7c00
ESP32               equ 0x7c00
RSP64               equ 0x7c00

video_info          equ 0xa000
memory_info         equ 0xb000

; page tables
PLL4T               equ 0xc000          ; page map level 4 table
PDPT                equ 0xd000          ; page directory pointer table
PDT                 equ 0xe000          ; page directory table
PT                  equ 0xf000          ; page table


