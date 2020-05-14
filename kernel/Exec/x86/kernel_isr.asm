;FLAG_INTERRUPT	equ 0x0e
;FLAG_R0             equ 0<<5            ; RINGS 0-3
;FLAG_P              equ 1<<7
;VIRT_BASE           equ 0
;CODE_SEL_64         equ 1

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

                    
IDT_ENTRIES         equ 256
                    section .text

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
endstruc

                    global current_task
current_task        dq 0

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


; each of the isr handlers pushes a word of it's IRQ number and jumps here
; this code puashes all the registers on the stack, and calls our single C IRQ handler
; the C IRQ handler expects this specific order of items on the stack!  See the ISR_REGISTERS struct.
                    global isr_common
isr_common:
                    push rdi            ; save rdi so we don't clobber it

                    mov rdi, [current_task]
                    mov [rdi + TASK.isrnum], rax            ; isrnum was pushed on stack by xisr


                    ; set default value for task_error_code
                    xor rax, rax
                    mov [rdi + TASK.error_code], rax

                    ; stack is rdi, rax

                    ; rdi is on the stack
;                    pop rax                                 ; restore saved rdi from the macro 
;                    mov [rdi + TASK.rdi], rax

;                    mov rax, [rdi + TASK.rdi]
;                    push rax

                    push rbx
                    push rcx
                    push rdx
                    push rsi
                    push r8
                    push r9
                    push r10
                    push r11
                    push r12
                    push r13
                    push r14
                    push r15
                    push rbp

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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                    mov [rdi + TASK.rsp], rsp
                    ; pass isrnum to C method as argument
                    mov rdi, [rdi + TASK.isrnum]
                    call kernel_isr

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                    ; restore task state
                    mov rdi, [current_task]
                    mov rsp, [edi + TASK.rsp]

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

                    popf
                    ret

                    global enter_tasking
enter_tasking:
                    jmp $
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

;; the %rep here generates 256 handlers inline!
ISRS:
                    %assign i 0
                    %rep IDT_ENTRIES
                    ISR i
                    %assign i (i+1)
                    %endrep


                    global gdt_flush

; called as: gdt_flush(&gdt_ptr) in C
gdt_flush:          mov rbx, [esp]              ; return address
;                    bochs
                    mov rax,rdi 
                    lgdt [rax]                      ; load the gdt into the CPU
                    mov eax, 0x10                   ; 0x10 is the offset to the gdt data segment entry
;                    mov ds, ax                      ; setup all segment registers to 0x10, we do not need them!
;                    mov es, ax                      ; segmentation is NOT used anymore
;                    mov fs, ax
;                    mov gs, ax
                    mov ss, ax
                    mov rax, rbx
                    jmp rax
;                    push .flush
;                    push 0x08
;                    ret
;                    jmp 0x08:.flush                 ; long jump to apply the new GDT
.flush:                                           ; simple long jump which returns from the function in C
                    ret

                    global tss_flush
tss_flush:          mov eax, 0x28
                    ltr ax                       ; "load task register"
.flush:
                    ret

                    global load_page_directory
load_page_directory:
                    mov cr3, rdi
                    ret

                    global enable_paging
enable_paging:
                    push rax
                    push rbx
                    mov rax, cr0
                    mov rbx, 80000000h
                    or  rax, rbx
                    mov cr0, rax
                    pop rbx
                    pop rax
                    ret

                    global set_vector
set_vector:
                    mov rax, rdi
                    ret

                    global load_idtr
                    extern idt_p
load_idtr:
                    lidt [rdi]
                    ret

                    global schedule_trap 
schedule_trap:
                    int 48
                    ret

                    global cli
cli:
                    cli
                    ret

                    global sti
sti:
                    sti
                    ret

                    global GetCS
GetCS:
                    xor rax, rax
                    mov ax, cs
                    ret

                    global GetSS
GetSS:
                    xor rax, rax
                    mov ax, ss
                    ret

                    global GetDS
GetDS:
                    xor rax, rax
                    mov ax, ds
                    ret

                    global GetES
GetES:
                    xor rax, rax
                    mov ax, es
                    ret

                    global GetFS
GetFS:
                    xor rax, rax
                    mov ax, fs
                    ret

                    global GetGS
GetGS:
                    xor rax, rax
                    mov ax, gs
                    ret

                    global GetRFLAGS
GetRFLAGS:
                    pushf
                    pop rax
                    ret

                    global GetFlags
GetFlags:
                    pushf
                    pop rax
                    ret

                    global SetFlags
SetFlags:
                    push rdi
                    popf
                    ret

                    global eputs
eputs               pushf
                    cli
                    push dx
                    mov dx, 0xe9
.loop:
                    mov al, [rdi]
                    add rdi, 1
                    test al, al
                    je .done
                    out dx, al
                    jmp .loop
.done:
                    pop dx
                    popf
                    ret

                    global ack_irq8
ack_irq8:
                    push rax
                    ; ack RTC
                    mov al, 0x0c
                    out 0x70, al
                    in al, 0x71

                    ; ack PIC
                    mov al, 0x20
                    out 0xa0, al
                    out 0x20, al

                    pop rax
                    ret

                    global enable_irq8
enable_irq8:
                    ; enable IRQ8 (IRQ0 on PIC2)
                    push rax
                    push rbx
;                    in al, 0xa1
;                    and al, 11111110b
;                    out 0xa1, al

	in al, 0xA1
	mov al, 11111110b		; Enable RTC interrupt
	out 0xA1, al

	; Set the periodic flag in the RTC
	mov al, 0x0B			; Status Register B
	out 0x70, al			; Select the address
	in al, 0x71			; Read the current settings
	push rax
	mov al, 0x0B			; Status Register B
	out 0x70, al			; Select the address
	pop rax
	bts ax, 6			; Set Periodic(6)
	out 0x71, al			; Write the new settings

%if 0
                    ; enable RTC
                    mov al, 8bh
                    out 70h, al
                    in al, 71h
                    or al, 01000000b
                    and al, 11001111b
                    xchg bl, al
                    mov al, 8bh
                    out 70h, al
                    xchg bl, al
                    out 71h, al

                    ;set RTC frequency
                    mov al, 8ah
                    out 70h, al
                    in al, 71h
                    and al, 0xf0
                    or al, 06h   ;1024Hz
;                    or al, 0eh   ;256hz
                    xchg al, bl
                    mov al, 8ah
                    out 70h, al
                    xchg al, bl
                    out 71h, al
%endif
	; Acknowledge the RTC
	mov al, 0x0C			; Status Register C
	out 0x70, al			; Select the address
	in al, 0x71			; Read the current settings
                    pop rbx
                    pop rax
                    ret
;                    jmp ack_irq8

                    global push_flags
push_flags:
                    pop rax
                    pushf
                    jmp rax

                    global pop_flags
pop_flags:
                    pop rax
                    popf
                    jmp rax

                    global push_disable
push_disable:
                    pop rax
                    pushf
                    cli
                    jmp rax

                    global pop_disable
pop_disable:
                    pop rax
                    popf
                    jmp rax

                    global pic_100hz
pic_100hz:
                    push rdx
                    mov dx, 1193180 / 100
                    mov al, 110110
                    out 0x43, al

                    mov ax, dx
                    out 0x40, al
                    xchg ah, al
                    out 0x40, al
                    pop rdx
                    ret

                    global mouse_trap
MOUSE_TRAP          equ 0x20 + 12
mouse_trap:
                    int MOUSE_TRAP
                    ret

                    global rdrand
rdrand:             
                    rdrand rax     ;generate a 32 bit random number and store it in EAX
                    jc rdrand
                    ret

                    global task_switch
task_switch:
                    ret
