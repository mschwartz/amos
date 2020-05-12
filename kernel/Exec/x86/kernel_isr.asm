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
;                    push rax
;                    mov rax, rsp
;                    call hexquadn64
;                    pop rax


                    push rdi            ; save rdi so we don't clobber it
;                    xchg bx, bx

                    mov rdi, [current_task]
                    mov [rdi + TASK.isrnum], rax            ; isrnum was pushed on stack by xisr

                    ; set default value for task_error_code
                    xor rax, rax
                    mov [rdi + TASK.error_code], rax
                    pop rax                                 ; restore saved rax from the macro 

                    mov [rdi + TASK.rbp], rbp

                    ; store general purpose registers
                    mov [rdi + TASK.rax], rax
                    mov [rdi + TASK.rbx], rbx
                    mov [rdi + TASK.rcx], rcx
                    mov [rdi + TASK.rdx], rdx

                    mov [rdi + TASK.rsi], rsi
                    pop rax                                 ; this is the rdi we pushed at isr_common
                    mov [rdi + TASK.rdi], rax

                    mov [rdi + TASK.r8], r8
                    mov [rdi + TASK.r9], r9
                    mov [rdi + TASK.r10], r10
                    mov [rdi + TASK.r11], r11
                    mov [rdi + TASK.r12], r12
                    mov [rdi + TASK.r13], r13
                    mov [rdi + TASK.r14], r14
                    mov [rdi + TASK.r15], r15

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

;.ec_msg:            db 'ecode', 13, 10,0
.error_code:
;                    mov rsi, .ec_msg
;                    call puts64
                    pop rax ; get error_code
                    mov [rdi + TASK.error_code], rax

.frame              pop rax                                 ; task's RIP
                    mov [rdi + TASK.rip], rax
                    pop rax                                 ; task's CS
                    mov [rdi + TASK.cs], eax
                    pop rax                                 ; task's flags
                    mov [rdi + TASK.rflags], rax
                    pop rax                                 ; task's RSP
                    mov [rdi + TASK.rsp], eax
                    pop rax                                 ; task's SS
                    mov [rdi + TASK.ss], eax


                    mov [rdi + TASK.ds], ds
                    mov [rdi + TASK.es], es
                    mov [rdi + TASK.fs], fs
                    mov [rdi + TASK.gs], gs

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                    ; pass isrnum to C method as argument
                    mov rdi, [rdi + TASK.isrnum]
                    call kernel_isr

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;                    jmp enter_tasking

                    ; restore task state
                    push rax
                    push rdi
                    mov rdi, [current_task]
                    pop rax
                    mov [rdi + TASK.rdi], rax
                    pop rax

                    ; set up the return stack using the task's stack memory
                    mov ss, [rdi + TASK.ss]
                    mov rsp, [rdi + TASK.rsp]

                    ; we save/restore the return stack in case we switch stacks on a task switch
                    xor rax, rax
                    mov eax, [rdi + TASK.ss]
                    push rax

                    mov rax, [rdi + TASK.rsp]
                    push rax

                    mov rax, [rdi + TASK.rflags]
                    push rax

                    mov eax, [rdi + TASK.cs]
                    push rax

                    mov rax, [rdi + TASK.rip]
                    push rax

                    ; stack is now ready for iretq

                    mov rax, [rdi + TASK.rax]
                    mov rbx, [rdi + TASK.rbx]
                    mov rcx, [rdi + TASK.rcx]
                    mov rdx, [rdi + TASK.rdx]

                    mov r8, [rdi + TASK.r8]
                    mov r9, [rdi + TASK.r9]
                    mov r10, [rdi + TASK.r10]
                    mov r11, [rdi + TASK.r11]
                    mov r12, [rdi + TASK.r12]
                    mov r13, [rdi + TASK.r13]
                    mov r14, [rdi + TASK.r14]
                    mov r15, [rdi + TASK.r15]

                    mov rsi, [rdi + TASK.rsi]

                    mov gs, [rdi + TASK.gs]
                    mov fs, [rdi + TASK.fs]
                    mov es, [rdi + TASK.es]
                    mov ds, [rdi + TASK.ds]
                   
                    mov rbp, [rdi + TASK.rbp]
                    ; finally restore rdi (we don't need it anymore)
                    mov rdi, [rdi + TASK.rdi]

                    iretq


                    global enter_tasking
enter_tasking:
                    ; restore task state
                    mov rdi, [current_task]

                    ; set up the return stack using the task's stack memory
                    mov ss, [rdi + TASK.ss]
                    mov rsp, [rdi + TASK.rsp]

                    ; we save/restore the return stack in case we switch stacks on a task switch
;                    xor rax, rax
;                    mov eax, [rdi + TASK.ss]
;                    push rax

;                    mov rax, [rdi + TASK.rsp]
;                    push rax

;                    mov rax, [rdi + TASK.rflags]
;                    push rax

;                    mov eax, [rdi + TASK.cs]
;                    push rax

                    mov rax, [rdi + TASK.rip]
                    push rax

                    ; stack is now ready for iretq

                    mov rax, [rdi + TASK.rax]
                    mov rbx, [rdi + TASK.rbx]
                    mov rcx, [rdi + TASK.rcx]
                    mov rdx, [rdi + TASK.rdx]

                    mov r8, [rdi + TASK.r8]
                    mov r9, [rdi + TASK.r9]
                    mov r10, [rdi + TASK.r10]
                    mov r11, [rdi + TASK.r11]
                    mov r12, [rdi + TASK.r12]
                    mov r13, [rdi + TASK.r13]
                    mov r14, [rdi + TASK.r14]
                    mov r15, [rdi + TASK.r15]

                    mov rsi, [rdi + TASK.rsi]

                    mov gs, [rdi + TASK.gs]
                    mov fs, [rdi + TASK.fs]
                    mov es, [rdi + TASK.es]
                    mov ds, [rdi + TASK.ds]
                   
                    mov rbp, [rdi + TASK.rbp]
                    mov rdi, [rdi + TASK.rdi]

                    ret
;                    pop rax
;                    jmp rax
;                    iretq

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
                    ret

