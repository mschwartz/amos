;; kernel_tasking.asm %macro bochs 0 xchg bx, bx %endm %macro BOCHS 0 xchg bx, bx %endm ;; kernel_isr is the "C/C++" function to be called extern kernel_isr

%macro BOCHS 0
	xchg bx,bx
%endmacro
	
%macro bochs 0
	xchg bx,bx
%endmacro

struc TSS
.reserved1 resd 1
.rsp0      resq 1
.rsp1      resq 1
.rsp2      resq 1
.reserved2 resq 1
.ist1      resq 1
.ist2      resq 1
.ist3      resq 1
.ist4      resq 1
.ist5      resq 1
.ist6      resq 1
.ist7      resq 1
.reserved3 resq 1
.reserved4 resw 1
.io_mba    resw 1
endstruc

;; TASK structure MUST match the one in idt.h
struc TASK
.task               resq 1
.rip                resq 1 	; RIP of first instruction to execute for task
.rflags             resq 1	; initial rflags
.ksp                resq 1
.rsp                resq 1
.cr3                resq 1
.error_code         resq 1
.isrnum             resq 1
.fxsave             resb 512 + 16
endstruc


	
;; each of the isr handlers pushes a word of it's IRQ number and jumps here
;; this code puashes all the registers on the stack, and calls our single C IRQ handler
;; the C IRQ handler expects this specific order of items on the stack!  See the ISR_REGISTERS struct.
global isr_common
isr_common:
	; stack at this point:
	; +0x0000 RIP
	; +0x0008 CS
	; +0x0010 RFLAGS
	; +0x0018 RSP
	; +0x0020 SS
	; NOTE: rax pushed by ISR is at the very top, we must pop it
	push rdi            ; save rdi so we don't clobber it

	swapgs
        mov rdi, [gs:CURRENT_TASK]
	
        mov [rdi + TASK.isrnum], rax            ; isrnum was pushed on stack by xisr

        ; set default value for task_error_code
        xor rax, rax
        mov [rdi + TASK.error_code], rax


	; we want to store the entire register set in the current task struct
        ; stack is rdi, rax, then interrupt stack frame, per above
	pop rdi
	pop rax

	; stack at this point:
	; +0x0000 RIP
	; +0x0008 CS
	; +0x0010 RFLAGS
	; +0x0018 RSP
	; +0x0020 SS
	; NOTE: rax is now the value pushed by the isr

	push rsi
	push rdi
	push rbp


        mov rdi, [gs:CURRENT_TASK]
	swapgs


	; push general purpose registers
	push rax
	push rbx
	push rcx
	push rdx
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	; all general purpose registers are saved at this point
	xor rax, rax
	mov ax, ds
	push rax 		; ds
	mov ax, es
	push rax 		; es
	mov ax, fs
	push rax 		; fs

	; save coprocessor registers, if CPU has them
        mov rax, cr4
        bts rax, 9
        je .continue

        ; fxsave and fxrstor need to store/restore to/from
	; 16 byte aligned addresses
        mov rax, rdi
        add rax, TASK.fxsave
        add rax, 15
        and rax, ~0x0f
        fxsave [rax]

.continue:
        ; the CPU pushes an extra error code for certain interrupts
        mov rax, [rdi + TASK.isrnum]
        cmp rax, 8
        je .error_code
        cmp rax, 9
        je .error_code
        cmp rax, 10
        je .error_code
        cmp rax, 11 
        je .error_code
        cmp rax, 12 
        je .error_code
        cmp rax, 15 
        je .error_code
        jmp .frame

.error_code:
        pop rax ; get error_code
        mov [rdi + TASK.error_code], rax

.frame:
	mov [rdi + TASK.rsp], rsp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

extern kernel_isr
	
        ; pass isrnum to C method as argument
        mov rdi, [rdi + TASK.isrnum]
        call kernel_isr

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global restore_task_state
restore_task_state:
	cli
        ; restore task state
	swapgs
        mov rdi, [gs:CURRENT_TASK]
	mov rdx, [gs:CURRENT_TSS]
	swapgs
	mov rax, [rdi + TASK.ksp]
	mov [rdx + TSS.ist1], rax

	mov rsp, [rdi + TASK.rsp] ; task's kernel stack pointer

	; set cr3 to page tables for the (new) current task, if the page table is not the current one
	mov rax, [rdi + TASK.cr3]
	mov rbx, cr3
	cmp rax, rbx
	je .same
	mov cr3, rax
.same:

	; restore fpu and xmm (etc) state
        mov rax, cr4
        bts rax, 9
        je .continue

        finit
        ; fxsave and fxrstor need to store/restore to/from 16 byte aligned addresses
        mov rax, rdi
        add rax, TASK.fxsave
        add rax, 15
        and rax, ~0x0f
        fxrstor [rax]

.continue:
	; kernel stack is prepared for iretq and has registers pushed on it
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8

	pop rax
	mov fs, ax
	pop rax
	mov es, ax
	pop rax
	mov ds, ax

	pop rdx
	pop rcx
	pop rbx
	pop rax
	
	pop rbp
	pop rsi
	pop rdi

        iretq


global init_task_state
        ; rdi = struct to init
init_task_state:
        ; save flags on stack, we want to do this with interrupts disabled, since we are mucking with rsp
        pushf
        cli

	push rax
	push rcx

	mov rcx, rsp 		; save caller rsp

	mov rsp, [rdi + TASK.ksp] ; task's top of kernel stack

	; create stack for iretq:
	; +0x0000 RIP
	; +0x0008 CS
	; +0x0010 RFLAGS
	; +0x0018 RSP
	; +0x0020 SS

	xor rax, rax
	mov ax, ss
	push rax		; SS

	mov rax, [rdi + TASK.rsp]
	push rax		; RSP

	mov rax, [rdi + TASK.rflags]
	push rax		; RFLAGS
	
	xor rax, rax
	mov ax, cs
	push rax		; CS
	
	mov rax, [rdi + TASK.rip]
	push rax		; RIP

	; push initial register values on the kernel stack

	mov rax, [rdi + TASK.task]
	push rax		; rdi
	xor rax, rax
	push rax		; rsi
	push rax		; rbp

	push rax		; rax
	push rbx		; rbx
	push rcx		; rcx
	push rdx		; rdx
	
	mov ax, ds
	push rax		; ds
	mov ax, es
	push rax		; es
	mov ax, fs
	push rax		; fs

	push rax 		; r8
	push rax 		; r9
	push rax 		; r10
	push rax 		; r11
	push rax 		; r12
	push rax 		; r13
	push rax 		; r14
	push rax 		; r15

	mov [rdi+TASK.rsp], rsp
	
	; save coprocessor registers, if CPU has them
        mov rax, cr4
        bts rax, 9
        je .continue

        ; fxsave and fxrstor need to store/restore to/from
	; 16 byte aligned addresses
        mov rax, rdi
        add rax, TASK.fxsave
        add rax, 15
        and rax, ~0x0f
        fxsave [rax]

.continue:

	mov rsp, rcx 		; restore caller's stack
	pop rcx
	pop rax
	popf

	ret
	
        popf
        ret

global enter_tasking
enter_tasking:
        ; jmp $
	jmp restore_task_state
%if 0
        cli
        ; restore task state
	swapgs
        mov rdi, [gs:CURRENT_TASK]
	swapgs

        ; set up the return stack using the task's stack memory
	;                    mov ss, [rdi + TASK.ss]
        mov rsp, [rdi + TASK.rsp]

        pop rbp
        pop r15
        pop r14
        pop r13
        pop r12
        pop r11
        pop r10
        pop r9
        pop r8
        pop rsi
        pop rdx
        pop rcx
        pop rbx
        pop rax
        ; finally restore rdi (we don't need it anymore)
        pop rdi
        iretq
%endif
	
global save_rsp
save_rsp:
        pushf
        cli
        push rdi

	swapgs
        mov rdi, [gs:CURRENT_TASK]
	swapgs
        test rdi, rdi
        jne .save
        pop rdi
        popf
        ret
.save:
        push rax
        mov rax, rsp
        add rax, 8 + 8 + 8
        mov [rdi + TASK.rsp], rax
        pop rax
        pop rdi
        popf
        ret

;;
;; GS Register details
;;
;; The gs register value is unique per CPU (each CPU has its own registers!)
;; The CPU instance is stored in gs:TGS.current_cpu
;; The current_task variable is stored in gs:TGS.current_task
;;
;; We can't use the global current_task variable that we used to because each CPU has its own current task!
;; 

;; This is the per CPU local data
CURRENT_GS: equ 0
CURRENT_TASK: equ 8
CURRENT_CPU: equ 16
CURRENT_TSS: equ 24
	

;; extern "C" void write_msr(TUint64 aRegister, TUint64 aValue);
global write_msr
write_msr:
	push rcx
	push rdx
	
	mov rcx, rdi
	mov rax, rsi
	mov rdx, rsi
	shr rdx, 32
	wrmsr

	pop rdx
	pop rcx
	ret

;; extern "C" TUint64 read_msr(TUint64 aRegister);
global read_msr
read_msr:
	push rcx
	push rdx

	mov rcx, rdi
	rdmsr
	shl rdx, 32
	or rax, rdx

	pop rdx
	pop rcx
	ret
	
global swapgs
swapgs:
	swapgs
	ret
	
;; this is callable from C/C++ to set the GS register value
global SetGS
SetGS:
	pushf
	cli
	
	push rax
	push rcx
	push rdx

	mov rax, rdi
	mov rdx, rdi
	shr rdx, 32
	mov rcx, 0xc0000101
	wrmsr

	mov [gs:CURRENT_GS], rdi
	swapgs

	; mov rcx, 0xc0000102
	; wrmsr
	
	; swapgs

	mov rax, [gs:CURRENT_GS]

	; mov rcx, 0xc0000101
	; rdmsr

	pop rdx
	pop rcx
	pop rax
	popf
	
	ret

gs_flag:
	db 0
align 16
	
global GetGS
GetGS:
	pushf
	cli

	swapgs
	mov rax, [gs:CURRENT_GS]
	swapgs

	popf
	ret

global SetCPU
SetCPU:
	pushf
	cli

	swapgs
	mov [gs:CURRENT_CPU], rdi
	swapgs

	popf
	
	ret
	
global GetCPU
GetCPU:
	pushf
	cli

	swapgs
	mov rax, [gs:CURRENT_CPU]
	swapgs

	popf
	ret
	
global SetCurrentTask
SetCurrentTask:
	pushf
	cli

	swapgs
	mov [gs:CURRENT_TASK], rdi
	swapgs
	
	popf
	ret

global GetCurrentTask
GetCurrentTask:
	pushf
	cli
	swapgs
	mov rax, [gs:CURRENT_TASK]
	swapgs
	popf
	ret

