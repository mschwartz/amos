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
%include "../boot/memory.inc"

extern kernel_main

start_msg:          db 13, 10, 'kernel_start', 13, 10, 0

align 8

extern init_start
extern init_end
extern text_start
extern text_end
extern rodata_start
extern rodata_end
extern data_start
extern data_end
extern bss_start
extern bss_end
extern kernel_end
	
boot:
	; SSE code copied from OSDEV SSE page
	mov eax, 0x1
	cpuid
	test edx, 1<<25
	jz .noSSE
	;SSE is available
.noSSE:
	;now enable SSE and the like
	mov rax, cr0
	and ax, 0xFFFB		;clear coprocessor emulation CR0.EM
	or ax, 0x2		;set coprocessor monitoring  CR0.MP
	mov cr0, rax
	mov rax, cr4
	or ax, 3 << 9		;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
	mov cr4, rax

	fninit			;
%ifdef SERIAL
        call debug64_init
%endif
        mov rsi, start_msg
        call puts64

	;                    mov rsi, rodata_start
	;                    mov rcx, 64
	;                    call hexdump64

	mov rdi, sys_info
	mov rax, init_start
	mov [rdi + SYSINFO.init_start], rax
	mov rax, init_end
	mov [rdi + SYSINFO.init_end], rax
	mov rax, text_start
	mov [rdi + SYSINFO.text_start], rax
	mov rax, text_end
	mov [rdi + SYSINFO.text_end], rax
	mov rax, rodata_start
	mov [rdi + SYSINFO.rodata_start], rax
	mov rax, rodata_end
	mov [rdi + SYSINFO.rodata_end], rax
	mov rax, data_start
	mov [rdi + SYSINFO.data_start], rax
	mov rax, data_end
	mov [rdi + SYSINFO.data_end], rax
	mov rax, bss_start
	mov [rdi + SYSINFO.bss_start], rax
	mov rax, bss_end
	mov [rdi + SYSINFO.bss_end], rax
	mov rax, kernel_end
	mov [rdi + SYSINFO.kernel_end], rax

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
