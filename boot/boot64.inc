                    [bits 64]
                    jmp boot64

                    %include "debug.inc"

hb:
                    push rax
                    shr rax, 4
                    call hexnybble
                    pop rax
                    call hexnybble
                    ret
hw:
                    push rax
                    shr rax, 8
                    call hb
                    pop rax
                    call hb
                    ret
boot64:
                    cli
                    mov rax, 0xdeadbeef
                    call hw
                    jmp $
                    push rax
                    shr rax, 24
                    call hexnybble
                    pop rax
                    shr rax, 20
                    call hexnybble
                    jmp $
                    mov rsi, cmain
                    mov rcx, 16
                    call hexdump

WHITE_ON_BLACK      equ 0x0f
WHITE_ON_GREEN      equ 0x1e

;                    mov rdi, rsp
;                    push rdi
                    global call_main
call_main:
                    call cmain
                    jmp $
                    mov edi, 0xB8000
                    mov ah, WHITE_ON_BLACK
                    mov al, 'x'
;                    mov ax, 0x2F20
                    mov ecx, 25*80
                    rep stosw                     ; Clear the screen.
                    jmp $
