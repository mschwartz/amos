;; memory.inc

LOAD_SEGMENT        equ  0x1000

pIDT:               dw 7FFh         ; limit of 256 IDT slots
                    dd 0000h        ; starting at 0000

pGDT:               dw 17FFh        ; limit of 768 GDT slots
                    dd 0800h        ; starting at 0800h (after IDT)                    
%if 1

gdt_start:

gdt_null:
                    dd 0x0
                    dd 0x0

gdt_code:
                    dw 0xffff
                    dw 0x0
                    db 0x0
                    db 0b10011010
                    db 0b11001111
                    db 0x0

gdt_data:
                    dw 0xffff
                    dw 0x0
                    db 0x0
                    db 0b10010010
                    db 0b11001111
                    db 0x0

gdt_end:

gdt_descriptor:
                    dw gdt_end - gdt_start
                    dd gdt_start

CODE_SEG            equ gdt_code - gdt_start
DATA_SEG            equ gdt_data - gdt_start
%endif
init_memory:
%if 0
; set A20 line
        cli                     ; no more interuptions! :)
        xor cx, cx
clear_buf:
        in al, 64h              ; get input from keyboard status port
        test al, 02h            ; test the buffer full flag
        loopnz clear_buf        ; loop until buffer is empty
        mov al, 0D1h            ; keyboard: write to output port
        out 64h, al             ; output command to keyboard
clear_buf2:
        in al, 64h              ; wait 'till buffer is empty again
        test al, 02h
        loopnz clear_buf2
        mov al, 0dfh            ; keyboard: set A20
        out 60h, al             ; send it to the keyboard controller
        mov cx, 14h
wait_kbc:                       ; this is approx. a 25uS delay to wait
        out 0edh, ax            ; for the kb controler to execute our 
        loop wait_kbc           ; command.
        %endif
                    ; enable A20 gate
;                    mov ax, 0x2401
;                    int 0x15

;                    lidt [pIDT]
;                    lgdt [pGDT]
;                    mov eax, cr0            ; load the control register in
;                    or  al, 1               ; set bit 1: pmode bit
;                    mov cr0, eax            ; copy it back to the control register
;                    jmp $+2                 ; and clear the prefetch queue
;                    nop
;                    nop 
;                    cli

                    ret
