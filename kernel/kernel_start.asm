                    [bits 64]

WHITE_ON_BLACK      equ 0x0f
WHITE_ON_GREEN      equ 0x1e

                    section text
                    global _start
_start:
                    mov edi, 0xB8000
                    mov ah, WHITE_ON_BLACK
                    mov al, '.'
;                    mov ax, 0x2F20
                    mov ecx, 25*80
                    rep stosw                     ; Clear the screen.
                    jmp boot

                    global foo
foo:
                    nop
                    ret

                    extern kernel_main
boot:
;                    mov ax, 0xdead
;                    call hexword
;                    jmp $
                    push rbp
                    mov rbp, rsp
                    mov rax, 0xdeadbeef
                    push rax
                    call kernel_main
                    add esp, 8
                    leave
                    ret

;BLOCKS              equ 16
;                    times BLOCKS-($-$$) db 0	; Pad remainder of boot sector with 0s
