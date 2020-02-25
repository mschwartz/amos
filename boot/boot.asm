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

                    org 0x7c00


start:
                    jmp  0:main

                    %include "memory.inc"
;; these filled in by build-img tool
                    align 4
boot_sector:        dw 0
boot_sectors:       dw 0
kernel_sector:      dw 0
kernel_sectors:     dw 0
msg                 db 'here', 0
; Variables
ERROR               db "A20 Error!" , 0
ERROR_MSG           db "Error!" , 0
BOOT_DRIVE:         db 0


                    align 4
;                    %include "disk16.inc"
                    %include "cga.inc"

                    align 4
cls16:
                    mov di, 0
                    push VIDEO_MEMORY_SEG
                    pop es
                    mov ah, WHITE_ON_BLACK
                    mov al, '-'
                    mov ecx, 25*80
                    rep stosw
                    ret

; prints a null - terminated string pointed to by EBX
print_string :
                    pusha
                    push es                                 ;Save ES on stack and restore when we finish

                    push VIDEO_MEMORY_SEG                   ;Video mem segment 0xb800
                    pop es
                    xor di, di                              ;Video mem offset (start at 0)
print_string_loop :
                    mov al , [ bx ]                         ; Store the char at BX in AL
                    mov ah , GREEN_ON_BLACK                 ; Store the attributes in AH
                    cmp al , 0                              ; if (al == 0) , at end of string , so
                    je print_string_done                    ; jump to done
                    mov word [es:di], ax                    ; Store char and attributes at current character cell.
                    add bx , 1                              ; Increment BX to the next char in string.
                    add di , 2                              ; Move to next character cell in vid mem.
                    jmp print_string_loop                   ; loop around to print the next char.

print_string_done :
                    pop es
                    popa
                    ret 

main:
                    xor ax, ax
                    mov ss, ax
                    mov sp, SP16

                    mov ds, ax
                    mov es, ax
                    mov fs, ax
                    mov gs, ax

                    mov [BOOT_DRIVE], dl

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
                    mov [di+0], eax
                    mov [di+2], eax
                    mov [di+4], eax
                    mov [di+6], eax
                    mov [di+8], eax
                    mov [di+10], eax

%if 0
;                    push ebx
;                    pop es
                    mov di, memory_info
init_memory_loop:
                    mov eax, 0e820h
                    mov ecx, 24
                    mov [di+20], dword 1
                    xor ebx, ebx
                    mov edx, SMAP
                    int 15h
                    jc init_memory_done
                    test ebx, ebx
                    je init_memory_done
                    cmp edx, SMAP
                    jne init_memory_done
;                    jmp init_memory_done
                    add di, 24

                    jmp init_memory_loop
init_memory_done:
;                    add di, 24
                    xor eax, eax
                    mov [di+0], eax
                    mov [di+2], eax
                    mov [di+4], eax
                    mov [di+6], eax
                    mov [di+8], eax
                    mov [di+10], eax
%endif
;                    call cls16

                    ; initialize disk drive for reading rest of boot and the kernel
                    ; must be called before dl is modified!
;                    call init_disk

                    ;; load in second sector
                    mov dh, 0                               ; head 0
                    mov dl, [BOOT_DRIVE]
                    mov ch, 0                               ; cylender 0
                    mov cl, [boot_sector]                   ; start sector
                    mov bx, 0x7e00
                    mov ah, 0x02                            ; operation: read sectors
                    mov al, [boot_sectors]                               ; number of sectors to read
                    int 13h

;                    LOAD 0x7e00, 2, 4
;                    LOAD 0x7e00, [boot_sector], [boot_sectors]
;                    LOAD 0x7e00, 2, 4

                    ;; load in kernel
                    mov dh, 0                               ; head 0
                    mov dl, [BOOT_DRIVE]
                    mov ch, 0                               ; cylender 0
                    mov cl, [kernel_sector]                   ; start sector
                    mov bx, CMAIN 
                    mov ah, 0x02                            ; operation: read sectors
                    mov al, [kernel_sectors]                               ; number of sectors to read
                    int 13h

;                    LOAD CMAIN, [kernel_sector], [kernel_sectors]
;                    LOAD cmain, 4, 8

                    ; jump to start of 2nd sector
                    jmp boot2

	times 510-($-$$) db 0	; Pad remainder of boot sector with 0s
	dw 0xAA55		; The standard PC boot signature

; ---------------------------------------------------------------------------------------------
;; start of second sector


                    %include "gdt.inc"
;                    %include "a20.inc"
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

boot2:
                    cli

                    ; enable a20
                    in al, 92h
                    or al, 02h
                    out 92h, al

                    lgdt [gdtr]

                    mov eax, cr0
                    or al, 1
                    mov cr0, eax

                    jmp 8:b32
                    nop
                    nop


                    [bits 32]

                    %include "screen.inc"
;                    %include "debug.inc"
;                    %include "paging.inc"
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
                    %include "paging.inc"

                    [bits 64]
                    jmp go



go:                 cli
                    mov rsp, RSP64
                    mov rsp, rbp
;                    mov rax, CMAIN
;                    call hexword

                    %include "boot64.inc"

                    jmp $

;BLOCKS              equ 3*512
;                    times BLOCKS-($-$$) db 0	; Pad remainder of boot sector with 0s

