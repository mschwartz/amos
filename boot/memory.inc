;; memory.inc
; memory map

gigabyte            equ 0x40000000

; page tables
PLL4T               equ 0x1000          ; page map level 4 table
PDPT                equ 0x2000          ; page directory pointer table
PDT                 equ 0x3000          ; page directory table
PT                  equ 0x4000          ; page table

load_sector_buffer  equ 0x4800          ; 512 byte sector buffer for reading

video_info          equ 0x5000
memory_info         equ 0x6000
gdt32               equ 0x7000
BOOTSTRAP_ORG       equ 0x7c00


; kernel address (loaded here)
CMAIN               equ 0x9000

; display
CGA                 equ 0xb8000

; STACK POINTER in 16, 32, 64 bit modes
SP16                equ 0x7c00
ESP32               equ 0x7c00
RSP64               equ 0x7c00

