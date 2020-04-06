;; memory.inc
; memory map

gigabyte            equ 0x40000000

video_info          equ 0x5000

misc_buffer         equ 0x5800          ; 512 byte buffer for misc. use

;gdt32               equ 0x7000
BOOTSTRAP_ORG       equ 0x7c00



; kernel address (loaded here)
CMAIN               equ 0x10000

; display
CGA                 equ 0xb8000

; STACK POINTER in 16, 32, 64 bit modes
SP16                equ 0x7c00
ESP32               equ 0x7c00
RSP64               equ 0x7c00

memory_info         equ 0x9000

; page tables
PLL4T               equ 0xa000          ; page map level 4 table
PDPT                equ 0xb000          ; page directory pointer table
PDT                 equ 0xc000          ; page directory table
PT                  equ 0xd000          ; page table


