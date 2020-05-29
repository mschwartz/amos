;; kernel_utils.asm

%macro BOCHS 0
	xchg bx,bx
%endmacro
	
%macro bochs 0
	xchg bx,bx
%endmacro
global gdt_flush

	; called as: gdt_flush(&gdt_ptr) in C
gdt_flush:
	bochs
        ; mov rbx, [esp]              ; return address
	;                    bochs
        mov rax,rdi 
        lgdt [rax]                      ; load the gdt into the CPU
        mov eax, 0x10                   ; 0x10 is the offset to the gdt data segment entry
	                   mov ds, ax                      ; setup all segment registers to 0x10, we do not need them!
	                   mov es, ax                      ; segmentation is NOT used anymore
	;                    mov fs, ax
	;                    mov gs, ax
        mov ss, ax
	xor eax, eax
	mov fs, ax
	mov gs, ax
        ; mov rax, rbx
        ; jmp rax
	;                    push .flush
	;                    push 0x08
	;                    ret
	;                    jmp 0x08:.flush                 ; long jump to apply the new GDT
.flush:                                           ; simple long jump which returns from the function in C
        ret

global tss_flush
tss_flush:
        mov eax, 0x28
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
