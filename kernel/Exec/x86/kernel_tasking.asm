;; kernel_tasking.asm

%macro bochs 0
        xchg bx, bx
%endm

%macro BOCHS 0
        xchg bx, bx
%endm

;; kernel_isr is the "C/C++" function to be called
extern kernel_isr

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

;; task_t structure MUST match the one in idt.h
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
.tss                resq 1
.error_code         resq 1
.isrnum             resq 1
.cs                 resq 1
.ds                 resq 1
.es                 resq 1
.fs                 resq 1
.gs                 resq 1
.ss                 resq 1
.fxsave             resb 512 + 16
endstruc

global current_task
	current_task        dq 0

	; each of the isr handlers pushes a word of it's IRQ number and jumps here
	; this code puashes all the registers on the stack, and calls our single C IRQ handler
	; the C IRQ handler expects this specific order of items on the stack!  See the ISR_REGISTERS struct.
global isr_common
extern hexquadn64
extern hexquads64
isr_common:
	; stack at this point:
	; +0x0000 RIP
	; +0x0008 CS
	; +0x0010 RFLAGS
	; +0x0018 RSP
	; +0x0020 SS
	; NOTE: rax pushed by ISR is at the very top, we must pop it
	; bochs
	push rdi            ; save rdi so we don't clobber it

        mov rdi, [current_task]
        mov [rdi + TASK.isrnum], rax            ; isrnum was pushed on stack by xisr

        ; set default value for task_error_code
        xor rax, rax
        mov [rdi + TASK.error_code], rax


	; we want to store the entire register set in the current task struct
        ; stack is rdi, rax, then interrupt stack frame, per above
	pop rax
	mov [rdi + TASK.rdi], rax ; rdi
	pop rax
	mov [rdi + TASK.rax], rax ; rax
	; now stack is interrupt stack frame

	mov [rdi + TASK.rbx], rbx
	mov [rdi + TASK.rcx], rcx
	mov [rdi + TASK.rdx], rdx
	mov [rdi + TASK.rsi], rsi
	mov [rdi + TASK.r8], r8
	mov [rdi + TASK.r9], r9
	mov [rdi + TASK.r10], r10
	mov [rdi + TASK.r11], r11
	mov [rdi + TASK.r12], r12
	mov [rdi + TASK.r13], r13
	mov [rdi + TASK.r14], r14
	mov [rdi + TASK.r15], r15
	mov [rdi + TASK.rbp], rbp

	; all general purpose registers are saved at this point

	xor rax, rax
	mov ax, ds
	mov [rdi + TASK.ds], rax
	mov ax, es
	mov [rdi + TASK.es], rax
	mov ax, fs
	mov [rdi + TASK.fs], rax
	mov ax, gs
	mov [rdi + TASK.gs], rax

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
	; now we unpack the stack frame
	pop rax
	mov [rdi + TASK.rip], rax
	pop rax
	mov [rdi + TASK.cs], ax
	pop rax
	mov [rdi + TASK.rflags], rax
	pop rax
	mov [rdi + TASK.rsp], rax
	pop rax
	mov [rdi + TASK.ss], ax

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        ; pass isrnum to C method as argument
        mov rdi, [rdi + TASK.isrnum]
        call kernel_isr

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

global restore_task_state
restore_task_state:
        ; restore task state
        mov rdi, [current_task]

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
	; prepare stack for iretq
	mov rax, [rdi + TASK.ss]
	push rax

	mov rax, [rdi + TASK.rsp]
	push rax

	mov rax, [rdi + TASK.rflags]
	push rax

	mov rax, [rdi + TASK.cs]
	push rax

	mov rax, [rdi + TASK.rip]
	push rax

	; stack is prepared for iretq

	; now restore the segment registers
	mov rax, [rdi + TASK.ds]
	mov ds, ax
	mov rax, [rdi + TASK.es]
	mov es, ax
	mov rax, [rdi + TASK.fs]
	mov fs, ax
	mov rax, [rdi + TASK.gs]
	mov gs, ax

	; restore general purpose registers
	mov rbp, [rdi + TASK.rbp]
	mov r15, [rdi + TASK.r15]
	mov r14, [rdi + TASK.r14]
	mov r13, [rdi + TASK.r13]
	mov r12, [rdi + TASK.r12]
	mov r11, [rdi + TASK.r11]
	mov r10, [rdi + TASK.r10]
	mov r9, [rdi + TASK.r9]
	mov r8, [rdi + TASK.r8]
	mov rsi, [rdi + TASK.rsi]
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

%if 0
        ; push registers we use/modify onto caller's stack
        push rcx

        mov rcx, rsp                            ; save caller rsp

        ; set up task's stack
        mov rsp, [rdi + TASK.upper_sp]

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

        xor rax, rax
        push rax            ; rdi
        push rax            ; rax
        push rax            ; rbx
        push rax            ; rcx
        push rax            ; rdx
        push rax            ; rsi
        push rax            ; r8
        push rax            ; r9
        push rax            ; r10
        push rax            ; r11
        push rax            ; r12
        push rax            ; r13
        push rax            ; r14
        push rax            ; r15
        push rax            ; rbp

        mov [rdi + TASK.rsp], rsp
        ; restore caller rsp
        mov rsp, rcx

        pop rcx
%endif
        popf
        ret

global enter_tasking
enter_tasking:
        ; jmp $
	jmp restore_task_state
%if 0
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
        pop rdi
        iretq
%endif
	
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
