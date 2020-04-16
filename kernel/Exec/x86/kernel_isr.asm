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
task_next           equ 0
task_prev           equ task_next + 8
task_rflags         equ task_prev + 8
task_rax            equ task_rflags + 8
task_rbx            equ task_rax + 8
task_rcx            equ task_rbx + 8
task_rdx            equ task_rcx + 8
task_rsi            equ task_rdx + 8
task_rdi            equ task_rsi + 8
task_ds             equ task_rdi + 8
task_es             equ task_ds + 4
task_fs             equ task_es + 4
task_gs             equ task_fs + 4
task_cs             equ task_gs + 4
task_ss             equ task_cs + 4
task_rip            equ task_ss + 4
task_rsp            equ task_rip + 8
task_rbp            equ task_rsp + 8
task_error_code     equ task_rbp + 8
task_isr_num        equ task_error_code + 8
task_sizeof         equ task_isr_num + 8

;task0               resb task_sizeof

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
                    extern stackdump
                    global isr_common
isr_common:
;                    call stackdump

                    push rdi            ; save rdi so we don't clobber it
;                    xchg bx, bx

                    mov rdi, [current_task]
                    mov [rdi + task_isr_num], rax
                    ; set default value for task_error_code
                    xor rax, rax
                    mov [rdi + task_error_code], rax
                    pop rax             ; restore saved rax from the macro 

                    mov [rdi + task_rbp], rbp

                    ; store general purpose registers
                    mov [rdi + task_rax], rax
                    mov [rdi + task_rbx], rbx
                    mov [rdi + task_rcx], rcx
                    mov [rdi + task_rdx], rdx
                    ;
                    mov [rdi + task_rsi], rsi
                    pop rax             ; this is the rdi we pushed at isr_common
                    mov [rdi + task_rdi], rax

                    ; the CPU pushes an extra error code for certain interrupts
                    mov rax, [rdi+task_isr_num]
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
                    mov [rdi + task_error_code], rax

                    ;
.frame              pop rax             ; task's RIP
                    mov [rdi + task_rip], rax
                    pop rax             ; task's CS
                    mov [rdi + task_cs], eax
                    pop rax             ; task's rflags
                    mov [rdi + task_rflags], rax
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

                    global resume_task
resume_task:
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

                    mov rax, [rdi + task_rflags]
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

                    global test_trap 
test_trap:
;                    bochs
                    int 0x80
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
