;                    org 0x9000
                    [bits 64]

%define SERIAL
COM1                equ 0x3f8
                    %macro BOCHS 0
                    xchg bx,bx
                    %endmacro

WHITE_ON_BLACK      equ 0x0f
WHITE_ON_GREEN      equ 0x1e

                    section start
                    global _start
_start:
;                    mov edi, 0xB8000
;                    mov ah, WHITE_ON_BLACK
;                    mov al, '.'
;                    mov ecx, 25*80
;                    rep stosw                     ; Clear the screen.
                    jmp boot

                    %include "../boot/debug64.inc"
                    global foo
foo:
                    nop
                    ret

                    extern _init, _fini
                    extern kernel_main

start_msg:          db 'kernel_start', 13, 10, 0
boot:
%ifdef SERIAL
                    call debug64_init
%endif
                    mov rsi, start_msg
                    call puts64

                    push rbp
                    mov rbp, rsp
;                    call _init
                    mov rax, 0xdeadbeef
                    push rax
                    call kernel_main
                    add esp, 8
;                    call _fini
                    leave
                    ret

                    global sputc
sputc:
                    push rax
                    mov rax, rdi
                    call putc64
                    pop rax
                    ret

;; inputs:
;;   rdi = address of memory to zero
;;   rsi = number of bytes to zero
                    global bzero
bzero:              
                    push rsi
                    push rdi
                    push rcx

                    xor rax, rax
                    mov rcx, rsi
                    rep stosb


                    pop rcx
                    pop rdi
                    pop rsi
                    ret

                    global __stack_chk_fail
__stack_chk_fail:   ret

;BLOCKS              equ 16
;                    times BLOCKS-($-$$) db 0	; Pad remainder of boot sector with 0s
