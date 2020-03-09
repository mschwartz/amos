                    [bits 64]
                    jmp boot64

                    %include "debug64.inc"

;hb:
;                    push rax
;                    shr rax, 4
;                    call hexnybble
;                    pop rax
;                    call hexnybble
;                    ret
;hw:
;                    push rax
;                    shr rax, 8
;                    call hb
;                    pop rax
;                    call hb
;                    ret
;hl:
;                    push rax
;                    shr rax, 16
;                    call hw
;                    pop rax
;                    call hw
;                    ret
boot64:
                    cli
;                    mov rax, 0xdeadbeef
;                    call hl
;                    push rax
;                    shr rax, 24
;                    call hexnybble
;                    pop rax
;                    shr rax, 20
;                    call hexnybble
;                    mov rsi, CMAIN
;                    mov rcx, 16
;                    call hexdump


;                    mov rdi, rsp
;                    push rdi
                    global call_main
call_main:
cli
                    mov esi, CMAIN
                    mov ecx, 16
                    call hexdump
                    call CMAIN
                    cli
jmp $
;                    mov rsi, CMAIN
;                    mov rcx, 16
;                    call hexdump
                    jmp $
                    mov edi, 0xB8000
                    mov ah, WHITE_ON_BLACK
                    mov al, 'x'
;                    mov ax, 0x2F20
                    mov ecx, 25*80
                    rep stosw                     ; Clear the screen.
                    jmp $
