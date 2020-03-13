; /*
;  * Boot sector is 512 bytes long.  The first 510 bytes are code.  The final 2 bytes (word) must be 0xaa55
;  *
;  * Boot sector is written to the first 512 byte sector of floppy disk or USB drive or hard drive.  Typically,
;  * the boot sector will then load additional sectors (the OS) from disk using the BIOS calls.
;  *
;  * Boot sector is loaded into memory at physical address 0x7c00 in 16 bit (real) mode.  It is up to the OS to then
;  * set up interrupt vectors, interrupt routines, etc.,  and then put the CPU into 32 or 64 bit mode and then 
;  * start up the additional CPU cores.
;  *
;  * BIOS int 0x10 is used to perform console output
;  * BIOS int 0x13 is used to perform disk I/O
;  */
	BITS 16

                    %include "memory.inc"
                    %include "cga.inc"

                    %macro BOCHS 0
                    xchg bx,bx
                    %endmacro

; ---------------------------------------------------------------------------------------------
; ---------------------------------------------------------------------------------------------
; ---------------------------------------------------------------------------------------------

                    org BOOTSTRAP_ORG

start:
                    jmp  0:main

;; these filled in by build-img tool
                    align 4
boot_sector:        dw 0
boot_sectors:       dw 0
kernel_sector:      dw 0
kernel_sectors:     dw 0
;msg                 db 'here', 0
; Variables
BOOT_DRIVE:         db 0
loading_msg         db 'Loading...', 13, 10, 0
;memory_msg          db 'memory', 13, 10, 0

; ---------------------------------------------------------------------------------------------
; ---------------------------------------------------------------------------------------------
; ---------------------------------------------------------------------------------------------

                    align 4
                    global main
main:
                    xor ax, ax
                    mov ss, ax
                    mov sp, SP16

                    mov ds, ax
                    mov es, ax
                    mov fs, ax
                    mov gs, ax

                    mov [BOOT_DRIVE], dl

; enable A20
set_a20:
                    in al, 0x64
                    test al, 0x02
                    jnz set_a20
                    mov al, 0xd1
                    out 0x64, al
wait_a20:
                    in al, 0x64
                    test al, 0x02
                    jnz wait_a20
                    mov al, 0xdf
                    out 0x60, al

                    jmp load

load_error1:
                    mov si, emsg1
                    jmp load_error
emsg1               db 'e1', 0
                    align 4
load_error2:
                    mov si, emsg2
                    jmp load_error
emsg2               db 'e2', 0
                    align 4
load_error:
                    push si
                    call hexbyte16
                    call space16
                    mov al, [0x7c00]
                    call hexbyte16
                    call space16
                    mov al, [0x7c01]
                    call hexbyte16
                    call space16
                    pop si
                    call puts16
                    call newline16
                    jmp $

                    global load
load:
                    mov dh, 0                               ; head 0
                    mov dl, [BOOT_DRIVE]
                    mov cx, [boot_sector]                   ; start sector
                    mov ch, 0                               ; cylender 0
                    mov bx, 0x7e00
                    mov ax, [boot_sectors]                  ; number of sectors to read
                    mov ah, 0x02                            ; operation: read sectors
                    int 13h
                    jc load_error1
                    ;; jump to second sector of boot program
                    jmp boot2

                    [bits 16]
                    %include "debug16.inc"

	times 510-($-$$) db 0	; Pad remainder of boot sector with 0s
	dw 0xAA55		; The standard PC boot signature

; ---------------------------------------------------------------------------------------------
; ---------------------------------------------------------------------------------------------
; ---------------------------------------------------------------------------------------------

;; start of second sector


gdt_start:
                    dd 0                ; null descriptor--just fill 8 bytes
                    dd 0

gdt_code:
                    dw 0FFFFh           ; limit low
                    dw 0                ; base low
                    db 0                ; base middle
                    db 10011010b        ; access
                    db 11001111b        ; granularity
                    db 0                ; base high

gdt_data:
                    dw 0FFFFh           ; limit low (Same as code)
                    dw 0                ; base low
                    db 0                ; base middle
                    db 10010010b        ; access
                    db 11001111b        ; granularity
                    db 0                ; base high
end_of_gdt:

gdtr:
                    dw end_of_gdt - gdt_start - 1   ; limit (Size of GDT)
                    dd gdt_start        ; base of GDT

CODE_SEG            equ gdt_code - gdt_start
DATA_SEG            equ gdt_data - gdt_start

boot2:
                    ; load kernel to 0x10000
                    mov ax, 0x1000
                    mov es, ax
                    xor bx, bx
                    mov dh, 0                               ; head 0
                    mov dl, [BOOT_DRIVE]
                    mov cx, [kernel_sector]                   ; start sector
                    mov ch, 0                               ; cylender 0
                    mov ax, [kernel_sectors]                  ; number of sectors to read
                    mov ah, 0x02                            ; operation: read sectors
                    int 13h
                    jc .error
                    xor eax,eax
                    mov es, ax
                    ; jump to start of 2nd sector
                    jmp .done
.error:
                    xor eax, eax
                    mov es, ax
                    jmp load_error2
.done:
;
;;; Query memory chunks via BIOS
;
mmap_ent            equ memory_info             ; the number of entries will be stored at 0x8000
do_e820:
        mov di, memory_info+4          ; Set di to 0x8004. Otherwise this code will get stuck in `int 0x15` after some entries are fetched 
	xor ebx, ebx		; ebx must be 0 to start
	xor bp, bp		; keep an entry count in bp
	mov edx, 0x0534D4150	; Place "SMAP" into edx
	mov eax, 0xe820
	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes
	int 0x15
	jc short .failed	; carry set on first call means "unsupported function"
	mov edx, 0x0534D4150	; Some BIOSes apparently trash this register?
	cmp eax, edx		; on success, eax must have been reset to "SMAP"
	jne short .failed
	test ebx, ebx		; ebx = 0 implies list is only 1 entry long (worthless)
	je short .failed
	jmp short .jmpin
.e820lp:
	mov eax, 0xe820		; eax, ecx get trashed on every int 0x15 call
	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes again
	int 0x15
	jc short .e820f		; carry set means "end of list already reached"
	mov edx, 0x0534D4150	; repair potentially trashed register
.jmpin:
	jcxz .skipent		; skip any 0 length entries
	cmp cl, 20		; got a 24 byte ACPI 3.X response?
	jbe short .notext
	test byte [es:di + 20], 1	; if so: is the "ignore this data" bit clear?
	je short .skipent
.notext:
	mov ecx, [es:di + 8]	; get lower uint32_t of memory region length
	or ecx, [es:di + 12]	; "or" it with upper uint32_t to test for zero
	jz .skipent		; if length uint64_t is 0, skip entry
	inc bp			; got a good entry: ++count, move to next storage spot
	add di, 24
.skipent:
	test ebx, ebx		; if ebx resets to 0, list is complete
	jne short .e820lp
.e820f:
	mov [mmap_ent], bp	; store the entry count
	clc			; there is "jc" on end of list to this point, so the carry must be cleared
	jmp .done
.failed:
	stc			; "function unsupported" error exit
.done:

%if 0
SMAP                equ                 0x0534d4150
                    global init_memory
init_memory:        
;                    mov si, memory_msg
;                    call puts16
                    mov di, memory_info
                    xor ebx, ebx        ; initial continuation value
                    xor ebp, ebp
                    mov edx, SMAP
                    mov [es:di + 20], dword 1
                    mov ecx, 24
                    mov eax, 0xe820
                    int 15h
                    jc .error
                    or ebx,ebx
                    je .done
                   
                    pusha
                    mov si, di
                    mov cx, 32
                    call hexdump16
                    mov eax, memory_info
                    call hexlong16
                    call newline16
                    popa
.top:
                    add di, 24
                    mov edx, SMAP
                    mov ecx, 24
                    mov eax, 0xe820
                    int 15h
                    jc .error
                    or ebx,ebx
                    je .done

                    jmp .top
.error:
                    call cls16
                    jmp $
.done:
                    xor eax, eax
                    mov ecx, 8
                    rep stosd
%endif
                    cli
                    lgdt [gdtr]

                    mov eax, cr0
                    or al, 1
                    mov cr0, eax

                    jmp 8:b32
                    nop
                    nop


                    [bits 32]

                    %include "screen.inc"
cls32:
                    push edi
                    push 0
                    pop esi
                    mov edi, 0xb8000
                    mov ecx, 25*50
                    mov ah, GREEN_ON_BLACK
                    mov al, 'x'
                    rep stosw
                    pop edi
                    ret

b32:

                    mov ax, 16
                    mov ds, ax
                    mov es, ax
                    mov fs, ax
                    mov gs, ax

                    mov ebp, ESP32
                    mov esp, ebp

                    call screen_clear

                    ;; go into long mode
                    jmp enter_long_mode

; Global Descriptor Table (64-bit).

gdt64:
	dq 0x0000000000000000
	dq 0x00209b0000000000 ;// 64 bit ring0 code segment
	dq 0x0020930000000000 ;// 64 bit ring0 data segment
	dq 0x0020fb0000000000 ;// 64 bit ring3 code segment
	dq 0x0020f30000000000 ;// 64 bit ring3 data segment
	dq 0x0000000000000000 ;// reserve for TSS low
	dq 0x0000000000000000 ;// reserve for TSS high
	dq 0x00cf9b000000ffff ;// 32 bit ring0 code segment
	dq 0x00cf93000000ffff ;// 32 bit ring0 data segment
gdt_ptr:
	dw (gdt_ptr - gdt64 - 1)
	dq gdt64
gdt_ptr64:
	dw (gdt_ptr - gdt64 - 1)
	dq (gdt64)
%if 0
GDT64:                           
Null:               equ $ - GDT64                           ; The null descriptor.
                    dw 0xFFFF                               ; Limit (low).
                    dw 0                                    ; Base (low).
                    db 0                                    ; Base (middle)
                    db 0                                    ; Access.
                    db 1                                    ; Granularity.
                    db 0                                    ; Base (high).
CODE:               equ $ - GDT64                           ; The code descriptor.
                    dw 0                                    ; Limit (low).
                    dw 0                                    ; Base (low).
                    db 0                                    ; Base (middle)
                    db 10011010b                            ; Access (exec/read).
                    db 10101111b                            ; Granularity, 64 bits flag, limit19:16.
                    db 0                                    ; Base (high).
DATA:               equ $ - GDT64                           ; The data descriptor.
                    dw 0                                    ; Limit (low).
                    dw 0                                    ; Base (low).
                    db 0                                    ; Base (middle)
                    db 10010010b                            ; Access (read/write).
                    db 00000000b                            ; Granularity.
                    db 0                                    ; Base (high).
GDT64_Pointer:                                              ; The GDT-pointer.
                    dw $ - GDT64 - 1                        ; Limit.
                    dq GDT64                                ; Base.
%endif

PTE_PRESENT         equ 1<<0
PTE_WRITE           equ 1<<1
PTE_LARGE           equ 1<<7
PAGE_SIZE           equ 4096

setup_identity_tables:
                    mov edx, PLL4T
                    mov eax, PLL4T + PAGE_SIZE
                    or eax, PTE_PRESENT + PTE_WRITE

                    mov [edx], eax
                    mov [edx + 2048], eax
                    mov [edx + 4088], eax

                    add edx, PAGE_SIZE
                    add eax, PAGE_SIZE
                    mov [edx], eax
                    mov [edx + 4080], eax

                    add eax, PAGE_SIZE
                    mov [edx + 8], eax
                    mov [edx + 4088], eax

                    add eax, PAGE_SIZE
                    mov [edx + 16], eax
                    add eax, PAGE_SIZE
                    mov [edx+24], eax

                    add edx, PAGE_SIZE
                    mov eax, PTE_LARGE + PTE_PRESENT + PTE_WRITE
                    mov ecx, 2048
.loop:
                    mov [edx], eax
                    add eax, 0x200000
                    add edx, 8
                    loop .loop

                    ret

setup_page_tables:
                    xor eax, eax
                    mov ecx, 4096
                    rep stosd
                    mov edi, cr3

                    mov DWORD [edi], 0x2003
                    add edi, 0x1000
                    mov DWORD [edi], 0x3003
                    add edi, 0x1000
                    mov DWORD [edi], 0x4003
                    add edi, 0x1000

                    mov ebx, 03h
                    mov ecx, 512
.set_entry:
                    mov dword [edi], ebx
                    add ebx, 0x1000
                    add edi, 8
                    loop .set_entry

                    ret

                    align 4
enter_long_mode:
                    mov eax, cr0
                    and eax, ~(1<<31)
                    mov cr0, eax

;                    call setup_page_tables
                    call setup_identity_tables

                    mov edi, PLL4T
                    mov cr3, edi
                    ; enable PAE paging
                    mov eax, cr4
                    or eax, 1<<5
                    mov cr4, eax

                    ; set LM bit
                    mov ecx, 0xC0000080          ; Set the C-register to 0xC0000080, which is the EFER MSR.
                    rdmsr
                    or eax, 1<<8
                    wrmsr

                    mov eax, cr0
                    or eax, 1<< 31 | 1<<0
                    mov cr0, eax

;                    lgdt [GDT64_Pointer]
                    lgdt [gdt_ptr64]
                    jmp CODE_SEG:go64
                    nop
                    nop
                    nop
                    align 8

                    [bits 64]
                    %include "debug64.inc"
go64:
                    cli
                    mov ax, DATA_SEG
                    mov dx, ax
                    mov es, ax
                    mov fs, ax
                    mov gs, ax
                    mov ss, ax
                    mov rbp, RSP64
                    mov rsp, rbp


                    global call_main
call_main:
                    mov esi, CMAIN
                    mov ecx, 16
                    call hexdump
                    call CMAIN
                    jmp $

