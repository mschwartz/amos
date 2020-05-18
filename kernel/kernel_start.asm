;                    org 0x9000
                    [bits 64]

COM1                equ 0x3f8

                    %macro BOCHS 0
                    xchg bx,bx
                    %endmacro

                    section start
                    global _start
_start:
                    mov [system_info], rdi

                    mov al, [edi + SYS_BOCHS]
                    mov [bochs_present], al
                    jmp boot

                    global system_info
system_info         dq 0
                    global bochs_present
bochs_present       db 0

                    align 16
                    %include "../common/memory.inc"
                    %include "../common/system.inc"
                    %include "../common/debug64.inc"

                    extern kernel_main

start_msg:          db 13, 10, 'kernel_start', 13, 10, 0

                    align 8

                    extern init_start, rodata_start
boot:
                    call debug64_init
                    mov rsi, start_msg
                    call puts64

                    mov rsi, kernel_main
                    mov rcx, 32
                    call hexdump64

                    mov rdi, [system_info]
                    call kernel_main
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
