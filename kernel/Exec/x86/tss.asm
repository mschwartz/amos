;; tss.asm

STACKSIZE           equ 8192

tss:
reserved0           dd 0
rsp0                dq 0
rsp1                dq 0
rsp2                dq 0
reserved1           dq 0
ist1                dq 0
ist2                dq 0
ist3                dq 0
ist4                dq 0
ist5                dq 0
ist6                dq 0
ist7                dq 0
reserved2           dq 0
reserved3           dw 0
iopb                dw 0
TSS_size            equ $ - tss

                    global install_tss
install_tss:
                    xor rax, rax
                    mov [reserved0], rax
                    mov [reserved1], rax
                    mov [reserved2], rax
                    mov [reserved3], rax

                    mov rax, stack0 + STACKSIZE
                    mov [rsp0], rax

                    mov rax, stack1 + STACKSIZE
                    mov [rsp1], rax

                    mov rax, stack2 + STACKSIZE
                    mov [rsp2], rax

                    mov rax, stack3 + STACKSIZE
                    mov [ist1], rax

                    mov rax, stack4 + STACKSIZE
                    mov [ist2], rax

                    mov rax, stack5 + STACKSIZE
                    mov [ist3], rax

                    mov rax, stack6 + STACKSIZE
                    mov [ist4], rax

                    mov rax, stack7 + STACKSIZE
                    mov [ist5], rax

                    mov rax, stack8 + STACKSIZE
                    mov [ist6], rax

                    mov rax, stack9 + STACKSIZE
                    mov [ist7], rax

                    mov rax, TSS_size
                    mov [iopb], ax

                    xor rax, rax
                    mov rax, tss
                    ret


                    section .bss
stack0              resb STACKSIZE
stack1              resb STACKSIZE
stack2              resb STACKSIZE
stack3              resb STACKSIZE
stack4              resb STACKSIZE
stack5              resb STACKSIZE
stack6              resb STACKSIZE
stack7              resb STACKSIZE
stack8              resb STACKSIZE
stack9              resb STACKSIZE
