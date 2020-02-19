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

bootDrive:          db 0
                    align 4

main:
                    mov ax, 0
                    mov ss, ax
                    mov sp, 0xfffc

                    mov ds, ax
                    mov es, ax
                    mov fs, ax
                    mov gs, ax

                    mov [bootDrive],dl

                    ;; load in second sector, and C program sectors
                    mov ah, 0x02
                    mov al, 4           ; number of sectors
                    mov cx, 0x0002
                    xor dh, dh
                    mov bx, 0x7e00
                    int 0x13

                    ; jump to start of 2nd sector
                    jmp boot2

end                 equ $
	times 510-($-$$) db 0	; Pad remainder of boot sector with 0s
	dw 0xAA55		; The standard PC boot signature

;                    %include "lib/screen.inc"
                    %include "lib/memory.inc"

boot2:
                    cli

                    lgdt [gdt_descriptor]

                    xor ebx ,ebx
                    mov ah, 0eh
                    mov al, 'g'

                    mov eax, cr0
                    or eax, 0x1
                    mov cr0, eax

                    jmp CODE_SEG:b32
                    nop
                    nop


                    [bits 32]

VIDEO_MEMORY equ 0xb8000
;WHITE_ON_BLACK equ 0x0f

%if 0
                    %include "lib/screen.inc"

print32:
                    pusha
                    mov edx, VIDEO_MEMORY
.loop:
                    mov al, [ebx]
                    cmp al, 0
                    je .done
                    mov ah, WHITE_ON_BLACK
                    mov [edx], eax
                    add edx, 2
                    add ebx, 1
                    jmp .loop
.done:
                    popa
                    ret
%endif

b32:
                    mov ax, DATA_SEG
                    mov ds, ax
                    mov es, ax
                    mov fs, ax
                    mov gs, ax

                    mov ebp, 0x2000
                    mov esp, ebp

                    call cmain
                    jmp $


cmain:
