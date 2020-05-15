;                    org 0x9000
                    [bits 64]

;%define SERIAL
COM1                equ 0x3f8

                    %macro BOCHS 0
                    xchg bx,bx
                    %endmacro

                    section start
                    global _start
_start:
                    jmp boot

                    %include "../boot/debug64.inc"

                    extern kernel_main

start_msg:          db 13, 10, 'kernel_start', 13, 10, 0

                    align 8

                    extern init_start, rodata_start
boot:
%ifdef SERIAL
                    call debug64_init
%endif
                    mov rsi, start_msg
                    call puts64

;                    mov rsi, rodata_start
;                    mov rcx, 64
;                    call hexdump64

                    push rbp
                    mov rbp, rsp
                    mov rax, 0xdeadbeef
                    push rax
                    call kernel_main
                    add esp, 8
                    leave
                    ret

                    global sputc
sputc:
                    push rax
                    mov rax, rdi
                    call putc64
                    pop rax
                    ret

                    global sputs
sputs:
                    push rax
                    push rsi
                    mov rsi, rdi
                    call puts64
                    pop rsi
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
