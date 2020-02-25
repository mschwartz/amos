;; memory.inc
; memory map

gigabyte            equ 0x40000000
gdt32               equ 0x7000
memory_info         equ 0x5000


; kernel address (loaded here)
CMAIN               equ 0x9000

; display
CGA                 equ 0xb8000


; STACK POINTER in 16, 32, 64 bit modes
SP16                equ 0x6000
ESP32               equ 0x6000
RSP64               equ 0x6000

