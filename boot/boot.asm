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
msg                 db 'here', 0
; Variables
BOOT_DRIVE:         db 0
loading_msg         db 'Loading...', 13, 10, 0

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

;
;;; Query memory chunks via BIOS
;
SMAP                equ                 0x0534d4150
init_memory:        
                    xor ebx, ebx        ; initial continuation value
                    mov ebp, ebx
                    mov di, memory_info
                    mov edx, SMAP
                    mov ecx, 24
                    mov eax, 0xe820
                    int 15h
                    jc .error
                    or ebx,ebx
                    je .done

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
                    jmp $
;
; inputs:
;   cl = start sector
;   ebx = memory address
;
load_sector:
                    push ebx
                    mov ebx, load_sector_buffer
                    mov dh, 0                               ; head 0
                    mov dl, [BOOT_DRIVE]
                    mov ch, 0                               ; cylender 0
                    mov ah, 0x02
                    mov al, 1
                    int 13h
                    jc load_error2
                   
                    mov si, loading_msg
                    call puts16

                    ;; copy sector to destination
                    pop ebx
                    push ebx
                    mov eax, ebx
                    call hexlong16
                    call space16

                    pop ebx
                    mov esi, load_sector_buffer
                    mov edi, ebx
                    mov ecx, 512
                    rep movsb

                    ret

;
; inputs:
;   cx = start sector
;   ax = number of sectors
;   bx = memory address
;
load_sectors:
                    pusha
                    call load_sector
                    popa
                    add cx, 1
                    add bx, 512
                    sub ax, 1
                    jne load_sectors
                    ret
                    
                    global load
load:
;                    mov cx, [boot_sector]
;                    mov ax, [boot_sectors]
;                    mov bx, 0x7e00
;                    call load_sectors
                    ;; load in second sector
                    mov dh, 0                               ; head 0
                    mov dl, [BOOT_DRIVE]
                    mov ch, 0                               ; cylender 0
                    mov cl, [boot_sector]                   ; start sector
                    mov bx, 0x7e00
                    mov ah, 0x02                            ; operation: read sectors
                    mov al, 1
                    mov al, [boot_sectors]                  ; number of sectors to read
                    int 13h
                    jc load_error1

                    mov eax, CMAIN
                    call hexlong16
                    call space16
                    mov ax, [kernel_sector]
                    call hexword16
                    call space16
                    mov ax, [kernel_sectors]
                    call hexword16
                    call newline16

;                    xchg bx, bx
                    ;; load in kernel
                    mov cx, [kernel_sector]
                    xor eax, eax
                    mov ax, [kernel_sectors]
                    mov ebx, CMAIN
                    call load_sectors

                    ; jump to start of 2nd sector
                    jmp boot2

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

                    %include "debug16.inc"

boot2:
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

                    align 4
enter_long_mode:
                    mov eax, cr0
                    and eax, ~(1<<31)
                    mov cr0, eax

                    mov edi, PLL4T
                    mov cr3, edi
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

                    lgdt [GDT64_Pointer]
                    jmp CODE:go64
                    nop
                    nop
                    nop
                    align 8

                    [bits 64]
                    %include "debug64.inc"
go64:
                    cli
                    mov ax, DATA
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

