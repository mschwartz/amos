                    jmp enter_long_mode
; Global Descriptor Table (64-bit).
GDT64:                           
Null:               equ $ - GDT64                           ; The null descriptor.
                    dw 0xFFFF                               ; Limit (low).
                    dw 0                                    ; Base (low).
                    db 0                                    ; Base (middle)
                    db 0                                    ; Access.
                    db 1                                    ; Granularity.
                    db 0                                    ; Base (high).
CODE:               equ $ - GDT64                           ; The code descriptor.
                    dw 0                                    ; Limit (low).
                    dw 0                                    ; Base (low).
                    db 0                                    ; Base (middle)
                    db 10011010b                            ; Access (exec/read).
                    db 10101111b                            ; Granularity, 64 bits flag, limit19:16.
                    db 0                                    ; Base (high).
DATA:               equ $ - GDT64                           ; The data descriptor.
                    dw 0                                    ; Limit (low).
                    dw 0                                    ; Base (low).
                    db 0                                    ; Base (middle)
                    db 10010010b                            ; Access (read/write).
                    db 00000000b                            ; Granularity.
                    db 0                                    ; Base (high).
GDT64_Pointer:                                              ; The GDT-pointer.
                    dw $ - GDT64 - 1                        ; Limit.
                    dq GDT64                                ; Base.

                    align 4

return_address:     dq 0
enter_long_mode:
                    pop edx             ; return address
                    mov [return_address+4], edx
                    mov eax, cr0
                    and eax, ~(1<<31)
                    mov cr0, eax

                    mov edi, PLL4T
                    mov cr3, edi
                    xor eax, eax
                    mov ecx, 4096
                    rep stosd
                    mov edi, cr3

                    mov DWORD [edi], 0x2003
                    add edi, 0x1000
                    mov DWORD [edi], 0x3003
                    add edi, 0x1000
                    mov DWORD [edi], 0x4003
                    add edi, 0x1000

                    mov ebx, 03h
                    mov ecx, 512
.set_entry:
                    mov dword [edi], ebx
                    add ebx, 0x1000
                    add edi, 8
                    loop .set_entry

                    ; enable PAE paging
                    mov eax, cr4
                    or eax, 1<<5
                    mov cr4, eax

                    ; set LM bit
                    mov ecx, 0xC0000080          ; Set the C-register to 0xC0000080, which is the EFER MSR.
                    rdmsr
                    or eax, 1<<8
                    wrmsr

                    mov eax, cr0
                    or eax, 1<< 31 | 1<<0
                    mov cr0, eax

                    lgdt [GDT64_Pointer]
                    jmp CODE:Realm64
                    nop
                    nop
                    nop
                    align 8

                    [bits 64]
Realm64:
                    cli
                    mov ax, DATA
                    mov dx, ax
                    mov es, ax
                    mov fs, ax
                    mov gs, ax
                    mov ss, ax
                    call foo
                    jmp bar
foo:
                    call foo2
                    ret
foo2:
                    call foo3
                    ret
foo3:
;                    mov rdi, 0xB8000
;                    mov rax, 0x1F201F201F201F20   ; Set the A-register to 0x1F201F201F201F20.
;                    mov ecx, 500                  ; Set the C-register to 500.
;                    rep stosq                     ; Clear the screen.
                    ret
bar:

