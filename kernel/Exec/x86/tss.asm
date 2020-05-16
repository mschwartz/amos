
                    struc TSS
.reserved0          resd 0
.rsp0l              resd 0
.rsp0h              resd 0
.rsp1l              resd 0
.rsp1h              resd 0
.rsp2l              resd 0
.rsp2h              resd 0
.reserved1          resd 0
.reserved2          resd 0
.ist1l              resd 0
.ist1h              resd 0
.ist2l              resd 0
.ist2h              resd 0
.ist3l              resd 0
.ist3h              resd 0
.ist4l              resd 0
.ist4h              resd 0
.ist5l              resd 0
.ist5h              resd 0
.ist6l              resd 0
.ist6h              resd 0
.ist7l              resd 0
.ist7h              resd 0
.reserved3          resd 0
.reserved4          resd 0
.reserved5          resw 0
.iopb               resw 0
                    endstruc

gdt64:
	dq 0x0000000000000000
	dq 0x00209b0000000000 ;// 64 bit ring0 code segment
	dq 0x0020930000000000 ;// 64 bit ring0 data segment
	dq 0x0020fb0000000000 ;// 64 bit ring3 code segment
	dq 0x0020f30000000000 ;// 64 bit ring3 data segment
	dq 0x0000000000000000 ;// reserve for TSS low
	dq 0x0000000000000000 ;// reserve for TSS high
	dq 0x00cf9b000000ffff ;// 32 bit ring0 code segment
	dq 0x00cf93000000ffff ;// 32 bit ring0 data segment
gdt_ptr:
	dw (gdt_ptr - gdt64 - 1)
	dq gdt64

                    global install_gdt
install_gdt:
                    lgdt [gdt_ptr]
                    ret

                    global install_tss
install_tss:
                    mov rdi, tss
                    xor rax, rax
                    stosd

                    mov rax, rsp0
                    stosd
                    shr rax, 32
                    stosd

                    mov rax, rsp1
                    stosd
                    shr rax, 32
                    stosd

                    mov rax, rsp2
                    stosd
                    shr rax, 32
                    stosd

                    ; reserved
                    xor rax, rax
                    stosd
                    xor rax, rax
                    stosd

                    mov rax, ist1
                    stosd
                    shr rax, 32
                    stosd

                    mov rax, ist2
                    stosd
                    shr rax, 32
                    stosd

                    mov rax, ist3
                    stosd
                    shr rax, 32
                    stosd

                    mov rax, ist4
                    stosd
                    shr rax, 32
                    stosd

                    mov rax, ist5
                    stosd
                    shr rax, 32
                    stosd

                    mov rax, ist6
                    stosd
                    shr rax, 32
                    stosd

                    mov rax, ist7
                    stosd
                    shr rax, 32
                    stosd

                    ; reserved
                    xor rax, rax
                    stosd
                    xor rax, rax
                    stosd
                    stosw

                    mov rax, TSS_size
                    stosw

                    mov rax, tss
                    ret


                    section .bss
stack0              resb 8192
rsp0                equ $
stack1              resb 8192
rsp1                equ $
stack2              resb 8192
rsp2:               equ $
stack3              resb 8192
ist1:               equ $
stack4              resb 8192
ist2:               equ $
stack5              resb 8192
ist3:               equ $
stack6              resb 8192
ist4:               equ $
stack7              resb 8192
ist5:               equ $
stack8              resb 8192
ist6:               equ $
stack9              resb 8192
ist7:               equ $
; reserve room for the tss
                    global tss
tss:                resb TSS_size

