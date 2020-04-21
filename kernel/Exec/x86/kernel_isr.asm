FLAG_INTERRUPT	equ 0x0e
FLAG_R0             equ 0<<5            ; RINGS 0-3
FLAG_P              equ 1<<7
IDT_ENTRIES         equ 256
VIRT_BASE           equ 0
CODE_SEL_64         equ 1

                    section .text

                    ; kernel_isr is the "C/C++" function to be called
                    extern kernel_isr

; task_t structure MUST match the one in idt.h
task_t              equ 0
task_flags          equ task_t
task_rax            equ task_flags + 8
task_rbx            equ task_rax + 8
task_rcx            equ task_rbx + 8
task_rdx            equ task_rcx + 8
task_rsi            equ task_rdx + 8
task_rdi            equ task_rsi + 8

task_r8             equ task_rdi +8
task_r9             equ task_r8 + 8
task_r10            equ task_r9 + 8
task_r11            equ task_r10 + 8
task_r12            equ task_r11 + 8
task_r13            equ task_r12 + 8
task_r14            equ task_r13 + 8
task_r15            equ task_r14 + 8

task_rip            equ task_r15 + 8
task_rsp            equ task_rip + 8
task_rbp            equ task_rsp + 8
task_errcode        equ task_rbp + 8
task_isrnum         equ task_errcode + 8
task_errno          equ task_isrnum + 8
task_cs             equ task_errno + 4
task_ds             equ task_cs + 2
task_es             equ task_ds + 2
task_fs             equ task_es + 2
task_gs             equ task_fs + 2
task_ss             equ task_gs + 2
task_sizeof         equ task_ss + 2


%if 0
STRUC task_t
                                        ; flags
                    task_flags         resq 0
                    ; general purpose registers
                    task_rax           resq 0
                    task_rbx           resq 0
                    task_rcx           resq 0
                    task_rdx           resq 0
                    task_rsi           resq 0
                    task_rdi           resq 0
                                        ; instruction pointer
                    task_rip           resq 0
                                        ; stack
                    task_rsp           resq 0
                    task_rbp           resq 0

                    task_errcode       resq 0
                    task_isrnum        resq 0
                    task_errno         resd 0

                                        ; segment registers
                    task_cs            resw 0
                    task_ds            resw 0
                    task_es            resw 0
                    task_fs            resw 0
                    task_gs            resw 0
                    task_ss            resw 0
ENDSTRUC
%endif

;task0               resb task_sizeof

                    global current_task
current_task        dq 0
                    global next_task
next_task           dq 0

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
                    extern stackdump
                    global isr_common
isr_common:
;                    call stackdump

                    push rdi            ; save rdi so we don't clobber it
;                    xchg bx, bx

                    mov rdi, [current_task]
                    mov [rdi + task_isrnum], rax
                    ; set default value for task_error_code
                    xor rax, rax
                    mov [rdi + task_errcode], rax
                    pop rax             ; restore saved rax from the macro 

                    mov [rdi + task_rbp], rbp

                    ; store general purpose registers
                    mov [rdi + task_rax], rax
                    mov [rdi + task_rbx], rbx
                    mov [rdi + task_rcx], rcx
                    mov [rdi + task_rdx], rdx

                    mov [rdi + task_rsi], rsi
                    pop rax             ; this is the rdi we pushed at isr_common
                    mov [rdi + task_rdi], rax

                    mov [rdi + task_r8], r8
                    mov [rdi + task_r9], r9
                    mov [rdi + task_r10], r10
                    mov [rdi + task_r11], r11
                    mov [rdi + task_r12], r12
                    mov [rdi + task_r13], r13
                    mov [rdi + task_r14], r14
                    mov [rdi + task_r15], r15

                    ; the CPU pushes an extra error code for certain interrupts
                    mov rax, [rdi + task_isrnum]
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
                    mov [rdi + task_errcode], rax

                    ;
.frame              pop rax             ; task's RIP
                    mov [rdi + task_rip], rax
                    pop rax             ; task's CS
                    mov [rdi + task_cs], eax
                    pop rax             ; task's flags
                    mov [rdi + task_flags], rax
                    pop rax             ; task's RSP
                    mov [rdi + task_rsp], eax
                    pop rax             ; task's SS
                    mov [rdi + task_ss], eax


                    mov [rdi + task_ds], ds
                    mov [rdi + task_es], es
                    mov [rdi + task_fs], fs
                    mov [rdi + task_gs], gs

                    ; TODO check return value to see if task switch should be done
                    call kernel_isr

                    ; restore task state
                    mov rdi, [current_task]

                    ; set up the return stack using the task's stack memory
                    mov ss, [rdi + task_ss]
                    mov rsp, [rdi + task_rsp]

                    ; we save/restore the return stack in case we switch stacks on a task switch
                    xor rax, rax
                    mov eax, [rdi + task_ss]
                    push rax

                    mov rax, [rdi + task_rsp]
                    push rax

                    mov rax, [rdi + task_flags]
                    push rax

                    mov eax, [rdi + task_cs]
                    push rax

                    mov rax, [rdi + task_rip]
                    push rax

                    ; stack is now ready for iretq

                    mov rax, [rdi + task_rax]
                    mov rbx, [rdi + task_rbx]
                    mov rcx, [rdi + task_rcx]
                    mov rdx, [rdi + task_rdx]

                    mov r8, [rdi + task_r8]
                    mov r9, [rdi + task_r9]
                    mov r10, [rdi + task_r10]
                    mov r11, [rdi + task_r11]
                    mov r12, [rdi + task_r12]
                    mov r13, [rdi + task_r13]
                    mov r14, [rdi + task_r14]
                    mov r15, [rdi + task_r15]

                    mov rsi, [rdi + task_rsi]

                    mov gs, [rdi + task_gs]
                    mov fs, [rdi + task_fs]
                    mov es, [rdi + task_es]
                    mov ds, [rdi + task_ds]
                   
                    mov rbp, [rdi + task_rbp]
                    ; finally restore rdi (we don't need it anymore)
                    mov rdi, [rdi + task_rdi]

                    iretq


                    global enter_next_task
enter_next_task:
                    ; restore task state
                    mov rdi, [next_task]
                    mov [current_task], rdi

                    ; set up the return stack using the task's stack memory
                    mov ss, [rdi + task_ss]
                    mov rsp, [rdi + task_rsp]

                    ; we save/restore the return stack in case we switch stacks on a task switch
                    xor rax, rax
                    mov eax, [rdi + task_ss]
                    push rax

                    mov rax, [rdi + task_rsp]
                    push rax

                    mov rax, [rdi + task_flags]
                    push rax

                    mov eax, [rdi + task_cs]
                    push rax

                    mov rax, [rdi + task_rip]
                    push rax

                    ; stack is now ready for iretq

                    mov rax, [rdi + task_rax]
                    mov rbx, [rdi + task_rbx]
                    mov rcx, [rdi + task_rcx]
                    mov rdx, [rdi + task_rdx]

                    mov r8, [rdi + task_r8]
                    mov r9, [rdi + task_r9]
                    mov r10, [rdi + task_r10]
                    mov r11, [rdi + task_r11]
                    mov r12, [rdi + task_r12]
                    mov r13, [rdi + task_r13]
                    mov r14, [rdi + task_r14]
                    mov r15, [rdi + task_r15]

                    mov rsi, [rdi + task_rsi]

                    mov gs, [rdi + task_gs]
                    mov fs, [rdi + task_fs]
                    mov es, [rdi + task_es]
                    mov ds, [rdi + task_ds]
                   
                    mov rbp, [rdi + task_rbp]
                    mov rdi, [rdi + task_rdi]

                    iretq

%if 0
                    ; restore task state
                    ; set up the return stack using the task's stack memory
                    mov ax, [rdi + task_ss]
                    mov ss, ax
                    mov rsp, [rdi + task_rsp]

                    ; we save/restore the return stack in case we switch stacks on a task switch
                    xor rax, rax
                    mov eax, [rdi + task_ss]
                    push rax

                    mov rax, [rdi + task_rsp]
                    push rax

                    mov rax, [rdi + task_flags]
                    push rax

                    mov eax, [rdi + task_cs]
                    push rax

                    mov rax, [rdi + task_rip]
                    push rax

                    ; stack is now ready for iretq

                    mov rax, [rdi + task_rax]
                    mov rbx, [rdi + task_rbx]
                    mov rcx, [rdi + task_rcx]
                    mov rdx, [rdi + task_rdx]

                    mov rsi, [rdi + task_rsi]

                    mov gs, [rdi + task_gs]
                    mov fs, [rdi + task_fs]
                    mov es, [rdi + task_es]
                    mov ds, [rdi + task_ds]
                   
                    mov rbp, [rdi + task_rbp]
                    ; finally restore rdi (we don't need it anymore)
                    mov rdi, [rdi + task_rdi]

                    global dump_it
dump_it:
;bochs
;                    call stackdump
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
                    mov [rdi + task_rsp], rax
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
                    mov rax, rdi
                    lidt [rax]
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
                    ; ack PIC
                    mov al, 0x20
                    out 0xa0, al
                    out 0x20, al

                    ; ack RTC
                    mov al, 0x0c
                    out 0x70, al
                    in al, 0x71

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
                    pop rbx
                    pop rax
                    jmp ack_irq8

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

