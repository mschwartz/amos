IDT_ENTRIES         equ 256
                    section .text

                    extern isr_common

                    %macro bochs 0
                    xchg bx, bx
                    %endm

                    %macro ISR 1
                    global xisr%1
xisr%1:
                    cli
                    push rax
                    mov rax, %1
                    jmp isr_common

                    global isr%1
isr%1:              dq xisr%1
                    %endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;; the %rep here generates 256 handlers inline!
ISRS:
                    %assign i 0
                    %rep IDT_ENTRIES
                    ISR i
                    %assign i (i+1)
                    %endrep


