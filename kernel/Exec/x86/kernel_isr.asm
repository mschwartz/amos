	;FLAG_INTERRUPT	equ 0x0e
	;FLAG_R0             equ 0<<5            ; RINGS 0-3
	;FLAG_P              equ 1<<7
	;VIRT_BASE           equ 0
	;CODE_SEL_64         equ 1

%if 0
extern putc64
extern puts64
extern newline64
extern space64
extern hexbyte64
extern hexword64
extern hexbytes64
extern hexwords64
extern hexlongs64
extern hexquads64
extern hexbyten64
extern hexwordn64
extern hexlongn64
extern hexquadn64
extern hexdump64
extern stackdump64
%endif
        
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
	isr%1:
	dq xisr%1
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


