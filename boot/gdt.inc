gdt_start:
                    dd 0                ; null descriptor--just fill 8 bytes
                    dd 0

gdt_code:
                    dw 0FFFFh           ; limit low
                    dw 0                ; base low
                    db 0                ; base middle
                    db 10011010b        ; access
                    db 11001111b        ; granularity
                    db 0                ; base high

gdt_data:
                    dw 0FFFFh           ; limit low (Same as code)
                    dw 0                ; base low
                    db 0                ; base middle
                    db 10010010b        ; access
                    db 11001111b        ; granularity
                    db 0                ; base high
end_of_gdt:

gdtr:
                    dw end_of_gdt - gdt_start - 1   ; limit (Size of GDT)
                    dd gdt_start        ; base of GDT

CODE_SEG            equ gdt_code - gdt_start
DATA_SEG            equ gdt_data - gdt_start

                    align 4
init_gdt32:
                    lgdt [gdtr]
                    ret

                    mov ax, 0
                    mov es, ax
                    mov di, gdt32

                    ; null descriptor
                    mov cx, 4
                    rep stosw

                    ; code segment
                    mov  WORD [di + 0], 0xffff              ; 4GB limit
                    mov  WORD [di + 2], 0x0000              ; base = 0
                    mov  BYTE [di + 4], 0x00                ; base
                    mov  BYTE [di + 5], 0x9a                ; access
                    mov  BYTE [di + 6], 0xcf                ; flags + limit = 0xcf
                    mov  BYTE [di + 7], 0x00                ; base


                    ; data segment
                    add di, 8
                    mov  WORD [di + 0], 0xffff              ; 4GB limit
                    mov  WORD [di + 2], 0x0000              ; base = 0
                    mov  BYTE [di + 4], 0x00                ; base
                    mov  BYTE [di + 5], 0x92                ; access
                    mov  BYTE [di + 6], 0xcf                ; flags + limit = 0xcf
                    mov  BYTE [di + 7], 0x00                ; base

                    lgdt [gdt32]
                    ret
igdt:
                    dw 24
                    dd gdt32

