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

cmain               equ 0x9000
RSP64               equ 0x6000

start:
                    jmp  0:main

;; these filled in by build-img tool
                    align 4
boot_sector:        dw 0
boot_sectors:       dw 0
kernel_sector:      dw 0
kernel_sectors:     dw 0

                    %include "disk.inc"
main:
                    mov ax, 0
                    mov ss, ax
                    mov sp, 0xfffc

                    mov ds, ax
                    mov es, ax
                    mov fs, ax
                    mov gs, ax

                    ; initialize disk drive for reading rest of boot and the kernel
                    ; must be called before dl is modified!
                    call init_disk

                    ;; load in second sector, and C program sectors
                    LOAD 0x7e00, 2, 2

                    ;; load in kernel
                    LOAD cmain, 4, 4

                    ; jump to start of 2nd sector
                    jmp boot2

end                 equ $
	times 510-($-$$) db 0	; Pad remainder of boot sector with 0s
	dw 0xAA55		; The standard PC boot signature

                    %include "memory.inc"
boot2:
                    cli


                    lgdt [gdt_descriptor]

                    mov eax, cr0
                    or eax, 0x1
                    mov cr0, eax

                    jmp CODE_SEG:b32
                    nop
                    nop


                    [bits 32]

;                    %include "debug.inc"
;                    %include "paging.inc"
b32:
                    mov ax, DATA_SEG
                    mov ds, ax
                    mov es, ax
                    mov fs, ax
                    mov gs, ax

                    mov ebp, 0x8000
                    mov esp, ebp

                    ;; go into long mode
                    %include "paging.inc"

                    [bits 64]


WHITE_ON_BLACK      equ 0x0f
WHITE_ON_GREEN      equ 0x1e

                    cli
;                    mov rsp, RSP64
;                    mov rsp, rbp

                    mov rsi, cmain
                    mov rcx, 16
                    call hexdump
jmp $
                    mov rdi, rsp
                    push rdi
                    call cmain
                    mov edi, 0xB8000
                    mov ah, WHITE_ON_BLACK
                    mov al, '.'
;                    mov ax, 0x2F20
                    mov ecx, 25*80
                    rep stosw                     ; Clear the screen.
                    jmp $

                    %include "debug.inc"

;BLOCKS              equ 3*512
;                    times BLOCKS-($-$$) db 0	; Pad remainder of boot sector with 0s

