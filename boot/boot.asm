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

                    %macro debugger 0
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
bochs_present:      db 0
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

                    ; detect running in bochs
detect_bochs:
                    mov dx, 0e9h
                    in al, dx
                    cmp al, 0e9h
                    jne .done
                    mov al, -1
                    mov [bochs_present], al
.done:

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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; query vesa graphics modes
; CAVEAT PROGRAMMER!  This code overwrites memory_info, so be sure to run this logic before setting up memory_info

                    struc vesa_info
.signature          resb 4
.version            resw 1
.oem                resd 1
.capabilities       resd 1
.video_modes        resd 1              ; segment:offset pointer to list of supported video modes
.video_memory       resw 1              ; amount of video memory in 64K blocks
.software_rev       resw 1
.vendor             resd 1
.product_name       resd 1
.product_rev        resd 1
.reserved           resb 222
.oem_data           resb 256+4
                    endstruc

                    ;; vbe_mode_info_structure
                    struc vesa_mode
.attributes         resw 1
.window             resw 1
;.window_a           resb 1
;.window_b           resb 1
.granularity        resw 1
.window_size        resw 1
.segment_a          resw 1
.segment_b          resw 1
.win_func_ptr       resd 1
.pitch              resw 1
.width              resw 1
.height             resw 1
.w_char             resb 1
.y_char             resb 1
.planes             resb 1
.depth              resb 1
.banks              resb 1
.memory_model       resb 1
.bank_size          resb 1
.image_pages        resb 1
.reserved0          resb 1
.red_mask           resb 1
.red_position       resb 1
.green_mask         resb 1
.green_position     resb 1
.blue_mask          resb 1
.blue_position      resb 1
.reserved_mask      resb 1
.reserved_position  resb 1
.direct_color_attributes resb 1
.framebuffer        resd 1
.off_screen_mem_off resd 1
.off_screen_mem_size resw 1
.reserved1          resb 206
                    endstruc

;; our display_mode struct 
                    struc display_mode
.mode               resw 1
.width              resw 1
.height             resw 1
.depth              resw 1
.pitch              resw 1
.pad                resw 1
.framebuffer        resd 1
.framebuffer_offset resd 1
.framebuffer_size   resw 1
.pad2               resw 1
                    endstruc

; layout our video_info memory so our kernel (C/C++) can access it
                    struc video_info_layout
                    resb video_info
.video_mode_count   resd 1
.display_info       resb vesa_info_size
.video_mode         resb display_mode_size
.video_modes        resb display_mode_size
                    endstruc

video_mode_count    equ video_info_layout.video_mode_count
display_info        equ video_info_layout.display_info
video_mode          equ video_info_layout.video_mode
video_modes          equ video_info_layout.video_modes

                    dd vesa_info_size
                    dd display_mode_size
                    dd video_mode_count
                    dd display_info
                    dd video_mode
                    dd video_modes

;; query vesa bios extensionf ro available modes
                    global get_vesa_modes
get_vesa_modes:
                    cli
                    ; zero our video_info memory
                    xor al, al
                    mov ecx, 1000h
                    mov edi, video_info
                    rep stosb

                    xor ecx, ecx
                    ; initialize mode count
                    mov [video_mode_count], ecx
                    ; initialize current mode to mode number 0 (undefined)
                    mov [video_mode + display_mode.mode], cx

                    ; get VBE info structure (vesa_*)
                    mov ah, 4fh
                    mov al, 00h
                    mov edi, display_info
                    int 10h

                    mov esi, [display_info + vesa_info.video_modes]             ; pointer to array of mode numbers
                    mov edi, video_modes                                        ; pointer to where we're goint to store our array
                    mov ebp, memory_info
.loop:
                    lodsw

                    mov [edi + display_mode.mode], ax                           ; store video mode
                    cmp ax, -1
                    je .done                                                    ; no more modes

                    ; get information about mode in ax to memory_info
                    pusha
                    mov cx, ax          ; mode
                    mov ah, 4fh
                    mov al, 1
                    mov edi, memory_info
                    int 10h
                    popa

                    ; copy mode info from memory_info to video_info
                    xor eax, eax
                    mov al, [ebp + vesa_mode.depth]
                    mov [edi + display_mode.depth], ax

                    mov ax, [ebp + vesa_mode.width]
                    mov [edi + display_mode.width], ax

                    mov ax, [ebp + vesa_mode.height]
                    mov [edi + display_mode.height], ax


                    mov ax, [ebp + vesa_mode.pitch]
                    mov [edi + display_mode.pitch], ax

                    mov eax, [ebp + vesa_mode.framebuffer]
                    mov [edi + display_mode.framebuffer], eax

                    mov eax, [ebp + vesa_mode.off_screen_mem_off]
                    mov [edi + display_mode.framebuffer_offset], eax

                    mov ax, [ebp + vesa_mode.off_screen_mem_size]
                    mov [edi + display_mode.framebuffer_size], ax


                    %if 0
                    pusha
                    mov esi, edi
                    mov ecx, display_mode_size
                    call hexdump16
                    mov esi, video_mode
                    mov ecx, display_mode_size
                    call hexdump16
                    call newline16
                    popa
                    %endif

                    ;; try to choose highest resolution 32 bit display
;                    BOCHS
                    mov ax, [video_mode + display_mode.mode]
                    test ax, ax
                    je .choose          ; no mode chosen yet, so choose this one

                    ; if new mode's height is greater than our chosen one, then we want to choose it
                    mov ax, [edi + display_mode.height]
                    cmp ax, [video_mode + display_mode.height]
                    ja .choose         ; higher resolution, use it
                    jne .next          ; lower resolution skip to next


                    ; same height, so see if new mode's width is higher resolution
                    mov ax, [edi + display_mode.width]
                    cmp ax, [video_mode + display_mode.width]
                    ja .choose         ; same height, higher width
                    jne .next           ; lower resolution, so skip it

                    ; same width and height, choose new mode if its depth is higher
                    mov ax, [edi + display_mode.depth]
                    cmp ax, [video_mode + display_mode.depth]
                    jl .next         ; lower bpp, skip it

                    ;; choose new mode as selected mode
.choose:
                    ; copy new mode's values to selected mode struc at video_mode
                    mov ax, [edi + display_mode.mode]
                    mov [video_mode + display_mode.mode], ax

                    mov ax, [edi + display_mode.width]
                    mov [video_mode + display_mode.width], ax

                    mov ax, [edi + display_mode.height]
                    mov [video_mode + display_mode.height], ax

                    mov ax, [edi + display_mode.depth]
                    mov [video_mode + display_mode.depth], ax

                    mov ax, [edi + display_mode.pitch]
                    mov [video_mode + display_mode.pitch], ax

                    mov eax, [edi + display_mode.framebuffer]
                    mov [video_mode + display_mode.framebuffer], eax

                    mov eax, [edi + display_mode.framebuffer_offset]
                    mov [video_mode + display_mode.framebuffer_offset], eax

                    mov ax, [edi + display_mode.framebuffer_size]
                    mov [video_mode + display_mode.framebuffer_size], ax

.next:
                    add edi, display_mode_size
                    inc ecx

                    jmp .loop
.done:
                    mov [video_mode_count], ecx
                    mov word [edi], -1

%ifdef KGFX
                    ; set video display mode
                    mov ah, 4fh
                    mov al, 02h
                    mov bx, [video_mode + display_mode.mode]
                    or bx, 1<<14        ; linear frame bufer select
                    int 10h
%endif


;
;;; Query memory chunks via BIOS
;
                    jmp probe_memory

                    ; ebx: which memory block t oget
                    ; edi: where to copy memory info
get_memory_block:
	mov eax, 0xe820
	mov [es:edi + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes
	mov edx, 0x0534D4150	; Place "SMAP" into edx
	int 0x15
                    jc .failed
	mov edx, 0x0534D4150	; Place "SMAP" into edx
	cmp eax, edx		; on success, eax must have been reset to "SMAP"
	jne .failed
	test ebx, ebx		; ebx = 0 implies list is only 1 entry long (worthless)
	je .failed
.skip               clc
                    ret
.failed             stc
                    ret

memory_count        equ memory_info
memory_infos        equ memory_count + 4

probe_memory:
                    xor ebx, ebx                            ; must be zero to start
                    mov [memory_count], ebx
                    mov edi, memory_infos
.loop:
                    call get_memory_block
                    jc .done

                    pusha
                    push edi

                    ; hex dump 24 bytes
;                    mov esi, edi
;                    mov ecx, 24
;                    call hexdump16
;                    call newline16

                    ; address 
                    mov edi, [esp]
                    mov eax, [edi + 4]
                    call hexlong16
                    mov edi, [esp]
                    mov eax, [edi + 0]
                    call hexlong16
                    call space16
                    ; length
                    mov edi, [esp]
                    mov eax, [edi + 12]
                    call hexlong16
                    mov edi, [esp]
                    mov eax, [edi + 8]
                    call hexlong16
                    call space16
                    ; type
                    mov edi, [esp]
                    mov al, [edi + 16]
                    call hexbyte16
                    call newline16
                    pop edi
                    popa

                    add edi, 24

                    mov eax, [memory_count]
                    add eax, 1
                    mov [memory_count], eax

                    jmp .loop
.done:
                    mov eax, [memory_count]

;                    mov esi, memory_info
;                    mov ecx, 32
;                    call hexdump16
;                    call newline16

%if 0

mmap_ent            equ memory_info                         ; the number of entries will be stored at 0x8000
do_e820:
                    mov di, memory_info+4                   ; Set di to 0x8004. Otherwise this code will get stuck in `int 0x15` after some entries are fetched 
	xor ebx, ebx		; ebx must be 0 to start
	xor ebp, ebp		; keep an entry count in bp
	mov edx, 0x0534D4150	; Place "SMAP" into edx
	mov eax, 0xe820
	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes
	int 0x15
	jc short .failed	                    ; carry set on first call means "unsupported function"
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
	inc ebp		; got a good entry: ++count, move to next storage spot
	add di, 24
.skipent:
	test ebx, ebx		; if ebx resets to 0, list is complete
	jne short .e820lp
.e820f:
	mov [mmap_ent], ebp	                    ; store the entry count
	clc		; there is "jc" on end of list to this point, so the carry must be cleared
	jmp .done
.failed:
	stc		; "function unsupported" error exit
.done:
%endif

%if 0
SMAP                equ                 0x0534d4150
                    global init_memory
init_memory:        
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

;                    debugger
;                    mov rsi, memory_info
;                    mov rcx, 16
;                    call hexdump

                    global call_main
call_main:
                    mov esi, CMAIN
                    mov ecx, 16
                    call hexdump
                    call CMAIN
                    jmp $

