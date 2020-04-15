;                    org 0x9000
                    [bits 64]

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

                    %include "Exec/x86/debug64.inc"
                    global foo
foo:
                    nop
                    ret

                    extern _init, _fini
                    extern kernel_main

boot:
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

                    global bzero
;; inputs:
;;   rdi = address of memory to zero
;;   rsi = number of bytes to zero
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
