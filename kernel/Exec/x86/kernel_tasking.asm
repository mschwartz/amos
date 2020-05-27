;; kernel_tasking.asm

KSTACK_SIZE: equ 16384		;
	
%macro bochs 0
        xchg bx, bx
%endm

%macro BOCHS 0
        xchg bx, bx
%endm

        ; kernel_isr is the "C/C++" function to be called
extern kernel_isr

	; task_t structure MUST match the one in idt.h
struc TASK 
.rflags             resq 1
.rax                resq 1
.rbx                resq 1
.rcx                resq 1
.rdx                resq 1
.rsi                resq 1
.rdi                resq 1
.r8                 resq 1
.r9                 resq 1
.r10                resq 1
.r11                resq 1
.r12                resq 1
.r13                resq 1
.r14                resq 1
.r15                resq 1
.rip                resq 1
.rsp                resq 1
.rbp                resq 1
.upper_sp           resq 1
.lower_sp           resq 1              ; used to save caller's stack in init_task_state
.error_code         resq 1
.isrnum             resq 1
.cs                 resw 1
.ds                 resw 1
.es                 resw 1
.fs                 resw 1
.gs                 resw 1
.ss                 resw 1
.fxsave             resb 512 + 16
endstruc

global current_task
	current_task        dq 0

	; each of the isr handlers pushes a word of it's IRQ number and jumps here
	; this code puashes all the registers on the stack, and calls our single C IRQ handler
	; the C IRQ handler expects this specific order of items on the stack!  See the ISR_REGISTERS struct.
global isr_common
isr_common:
	push rdi		                ; save rdi so we don't clobber it
        mov rdi, [current_task]
        mov [rdi + TASK.isrnum], rax            ; isrnum was pushed on stack by xisr
	pop rax					; get rdi off stack
	mov [rdi + TASK.rdi], rax		; save it in task struct

	pop rax					; rax was pushed in ISR handler
	mov [rdi + TASK.rax], rax		; store in task struct

	; save rest of registers in task struct
	mov [rdi + TASK.rbx], rbx
	mov [rdi + TASK.rcx], rcx
	mov [rdi + TASK.rdx], rdx
	mov [rdi + TASK.rsi], rsi
	mov [rdi + TASK.rbp], rbp
	mov [rdi + TASK.r8], r8
	mov [rdi + TASK.r9], r9
	mov [rdi + TASK.r10], r10
	mov [rdi + TASK.r11], r11
	mov [rdi + TASK.r12], r12
	mov [rdi + TASK.r13], r13
	mov [rdi + TASK.r14], r14
	mov [rdi + TASK.r15], r15

	; top of stack is possibly an error code, then rip

        ; set default value for task_error_code
        xor rax, rax
        mov [rdi + TASK.error_code], rax

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
	BOCHS			;
        pop rax ; get error_code
        mov [rdi + TASK.error_code], rax

.frame:
        mov rax, cr4
        bts rax, 9
        je .continue

        ; fxsave and fxrstor need to store/restore to/from 16 byte aligned addresses
        mov rax, rdi
        add rax, TASK.fxsave
        add rax, 15
        and rax, ~0x0f
        fxsave [rax]

.continue:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        mov [rdi + TASK.rsp], rsp
	mov rsp, kstack_top

        ; pass isrnum to C method as argument
        mov rdi, [rdi + TASK.isrnum]
        call kernel_isr

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global resume_task
resume_task:
	cli			; just to be sure if the ISR handler enaled interrupts

        ; restore task state
        mov rdi, [current_task]
        mov rsp, [edi + TASK.rsp]

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
	mov r15, [rdi + TASK.r15]
	mov r14, [rdi + TASK.r14]
	mov r13, [rdi + TASK.r13]
	mov r12, [rdi + TASK.r12]
	mov r11, [rdi + TASK.r11]
	mov r10, [rdi + TASK.r10]
	mov r9, [rdi + TASK.r9]
	mov r8, [rdi + TASK.r8]
	mov rsi, [rdi + TASK.rsi]
	mov rbp, [rdi + TASK.rbp]
	mov rdx, [rdi + TASK.rdx]
	mov rcx, [rdi + TASK.rcx]
	mov rbx, [rdi + TASK.rbx]
	mov rax, [rdi + TASK.rax]

        ; finally restore rdi (we don't need it anymore)
        mov rdi, [rdi + TASK.rdi]

        iretq

global init_task_state
        ; rdi = struct to init
init_task_state:
        ; save flags on stack, we want to do this with interrupts disabled, since we are mucking with rsp
        pushf
        cli

        ; push registers we use/modify onto caller's stack
        push rcx
        mov rcx, rsp                            ; save caller rsp

        ; set up task's stack
        mov rsp, [rdi + TASK.upper_sp]
        mov [rdi + TASK.rsp], rsp

        ; set up stack for iretq
        mov rax, 10h                             ; ss
        push rax

        mov rax, [rdi + TASK.upper_sp]          ; rsp
        push rax

        mov rax, [rdi + TASK.rflags]            ; rflags
        push rax

        mov rax, 8                              ; cs
        push rax

        mov rax, [rdi + TASK.rip]
        push rax

        ; push registers as if an IRQ/exception

        mov [rdi + TASK.rsp], rsp

        ; restore caller rsp
        mov rsp, rcx
        pop rcx

        popf
        ret

global enter_tasking
enter_tasking:
	jmp resume_task
;; jmp $
        cli
        ; restore task state
        mov rdi, [current_task]

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
	mov rdi, [rdi + TASK.rdi]
        iretq

global save_rsp
save_rsp:
        pushf
        cli
        push rdi

        mov rdi, [current_task]
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

section .bss
kstack:
	resb KSTACK_SIZE
kstack_top:
