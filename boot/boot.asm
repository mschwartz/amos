;; /*
;;  * Boot sector is 512 bytes long.  The first 510 bytes are code.  The final 2 bytes (word) must be 0xaa55
;;  *
;;  * Boot sector is written to the first 512 byte sector of floppy disk or USB drive or hard drive.  Typically,
;;  * the boot sector will then load additional sectors (the OS) from disk using the BIOS calls.
;;  *
;;  * Boot sector is loaded into memory at physical address 0x7c00 in 16 bit (real) mode.  It is up to the OS to then
;;  * set up interrupt vectors, interrupt routines, etc.,  and then put the CPU into 32 or 64 bit mode and then 
;;  * start up the additional CPU cores.
;;  *
;;  * BIOS int 0x10 is used to perform console output
;;  * BIOS int 0x13 is used to perform disk I/O
;;  */

BITS 16

COM1:   equ 0x3f8

%include "memory.inc"

;; when running in the bochs emulator, the xchg bx,bx command causes the emulator to stop
;; as if at a breakpoint.  When not in the emulator, xchg bx,bx is a noop, so
;; no harm is done.  Provide a BOCHS/bochs macro we can use to insert breakpoints in the source code.
%macro BOCHS 0
        xchg bx,bx
%endmacro

%macro bochs 0
        xchg bx,bx
%endmacro

;; ---------------------------------------------------------------------------------------------
;; ---------------------------------------------------------------------------------------------
;; ---------------------------------------------------------------------------------------------

;; This is the first instruction in the boot sector.  THe very first sector on the boot drive is
;; loaded at 0x7c00.  Since a sector is only 512 bytes, we're very limited to how much
;; code we can fit in and run.  Not a problem because we're going to use the BIOS
;; call to read in additional sectors.  So this file starts with the boot sector and
;; then is followed by as much code as we need.  This additional code is loaded immediately
;; after the boot sector (at 0x7c00 + 512 bytes).
org BOOTSTRAP_ORG

start:
        jmp 0:main		; this jmp instruction causes CS register to be 0

;; these filled in by build-img tool - do not MOVE or add variables before these.  The fixed offset
;; in the boot sector binary image matters.
align 4
boot_sector:        dw 0 	; sector number of the rest of the boot program
boot_sectors:       dw 0	; number of sectors of the rest of the boot program
kernel_sector:      dw 0	; sector number of the start of the kernel image on disk
kernel_sectors:     dw 0	; number of sectors that comprise the kernel image
root_sector:        dw 0	; root of file system

;; Anything past here doesn't matter to our build-img tool.
ebda:               dw 0
bochs_present:      db 0	; true if bochs is detected
serial_initialized: db 0
	
;; Variables
BOOT_DRIVE:         db 0	; boot sector is entered with the drive number of the boot device in dl
CPU_NUM:            db 0
loading_msg:        db 'Loading... ', 0

;; ---------------------------------------------------------------------------------------------
;; ---------------------------------------------------------------------------------------------
;; ---------------------------------------------------------------------------------------------

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
	mov ax, [040eh]
	mov [ebda], ax

        ; detect running in bochs
detect_bochs:
        mov dx, 0e9h
        in al, dx
        cmp al, 0e9h
        jne .done
        mov al, -1
        mov [bochs_present], al
.done:
        call debug16_init

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
        call hexwords16
        pop si
        call puts16
        call newline16
        jmp $

;; load rest of boot program
global load
load:
        mov si, loading_msg
        call puts16
        mov al, [BOOT_DRIVE]
        call hexbytes16
        mov al, [boot_sector]
        call hexbytes16
        mov ax, [boot_sectors]
        call hexwordn16

        mov dh, 0                               ; head 0
        mov dl, [BOOT_DRIVE]
        mov cx, [boot_sector]                   ; start sector
        mov ch, 0                               ; cylinder 0
        mov bx, 0x7e00
        mov ax, [boot_sectors]                  ; number of sectors to read
        mov ah, 0x02                            ; operation: read sectors
        int 13h
        jc load_error1
;; jump to second sector of boot program
        jmp boot2

[bits 16]
%include "debug16.inc"

;; Bytes 510 and 511 in the boot sector must be 0xaa55
	times 510-($-$$) db 0	; Pad remainder of boot sector with 0s
	dw 0xAA55		; The standard PC boot signature




;; ---------------------------------------------------------------------------------------------
;; ---------------------------------------------------------------------------------------------
;; ---------------------------------------------------------------------------------------------
;;
;; start of second sector (rest of boot program)
;;
;; ---------------------------------------------------------------------------------------------
;; ---------------------------------------------------------------------------------------------
;; ---------------------------------------------------------------------------------------------

[bits 16]
	times 512 db 90
	jmp 0:ap_boot
	
;; Global Descriptor Table (32-bit).
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

;; Global Descriptor Table (64-bit).
gdt64:                           
Null:   equ $ - gdt64                           ; The null descriptor.
        dw 0xFFFF                               ; Limit (low).
        dw 0                                    ; Base (low).
        db 0                                    ; Base (middle)
        db 0                                    ; Access.
        db 1                                    ; Granularity.
        db 0                                    ; Base (high).
CODE:   equ $ - gdt64                           ; The code descriptor.
        dw 0                                    ; Limit (low).
        dw 0                                    ; Base (low).
        db 0                                    ; Base (middle)
        db 10011010b                            ; Access (exec/read).
        db 10101111b                            ; Granularity, 64 bits flag, limit19:16.
        db 0                                    ; Base (high).
DATA:   equ $ - gdt64                           ; The data descriptor.
        dw 0                                    ; Limit (low).
        dw 0                                    ; Base (low).
        db 0                                    ; Base (middle)
        db 10010010b                            ; Access (read/write).
        db 00000000b                            ; Granularity.
        db 0                                    ; Base (high).
gdtr64:                                         ; The GDT-pointer.
        dw $ - gdt64 - 1                        ; Limit.
        dq gdt64                                ; Base.

CODE_SEG: equ gdt_code - gdt_start
DATA_SEG: equ gdt_data - gdt_start

;; APPLICATION PROCESSOR BOOT
ap_message:
	db 'AP BOOT ', 0
	ALIGN 4
ap_boot:
	cli
	mov al, [CPU_NUM]
	inc al
	mov [CPU_NUM], al

        xor ax, ax
        mov ss, ax
        mov sp, SP16

        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

	mov si, ap_message
	call puts16
	mov al, [CPU_NUM]
	call hexbyte16
	call newline16

	; enable A20
ap_set_a20:
        in al, 0x64
        test al, 0x02
        jnz ap_set_a20
        mov al, 0xd1
        out 0x64, al
ap_wait_a20:
        in al, 0x64
        test al, 0x02
        jnz ap_wait_a20
        mov al, 0xdf
        out 0x60, al

	; switch ap into 32-bit mode
        cli
        lgdt [gdtr]

	mov eax, cr0
	or al, 1
	mov cr0, eax
	jmp 8:ap_start32

	[bits 32]
align 16
ap_start32:
	mov ax, 0x10
	mov dx, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	xor eax, eax
	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx
	xor esi, esi
	xor edi, edi
	xor ebp, ebp
	mov esp, 0x7c000

;; enter 64 bit mode
	lgdt [gdtr64]
	; extendd properties
	mov eax, cr4
	or eax, 0xb0 		; PGE, PAE, PSE
	mov cr4, eax

	; use PML4 for paging
	mov eax, 0x2000
	mov cr3, eax
	
        ; Enable long mode and SYSCALL/SYSRET
	mov ecx, 0xC0000080		; EFER MSR number
	rdmsr				; Read EFER
	or eax, 0x00000101 		; LME (Bit 8)
	wrmsr				; Write EFER

        ; Enable paging to activate long mode
	mov eax, cr0
	or eax, 0x80000000		; PG (Bit 31)
	mov cr0, eax

	jmp CODE_SEG:ap_start64

;; CONTINUE BOOT CPU PROGRAM
	[bits 16]
boot2:
	; copy EBDA to ebda_temp
	; this needs to be done before loading kernel, which might overwrite this memory
	mov esi, 0x80000
	mov edi, ebda_temp
	mov ecx, 32768
	rep movsw
	
	jmp load2		; load the kernel
	
;; https://stackoverflow.com/questions/45434899/why-isnt-my-root-directory-being-loaded-fat12/45495410#45495410
;;
;;    Function: lba_to_chs
;; Description: Translate Logical block address to CHS (Cylinder, Head, Sector).
;;              Works for all valid FAT12 compatible disk geometries.
;;
;;   Resources: http://www.ctyme.com/intr/rb-0607.htm
;;              https://en.wikipedia.org/wiki/Logical_block_addressing#CHS_conversion
;;              https://stackoverflow.com/q/45434899/3857942
;;              Sector    = (LBA mod SPT) + 1
;;              Head      = (LBA / SPT) mod HEADS
;;              Cylinder  = (LBA / SPT) / HEADS
;;
;;      Inputs: SI = LBA
;;     Outputs: DL = Boot Drive Number
;;              DH = Head
;;              CH = Cylinder (lower 8 bits of 10-bit cylinder)
;;              CL = Sector/Cylinder
;;                   Upper 2 bits of 10-bit Cylinders in upper 2 bits of CL
;;                   Sector in lower 6 bits of CL
;;
;;       Notes: Output registers match expectation of Int 13h/AH=2 inputs
;;

;; these values consistent with 1.44MB floppy	
SectorsPerTrack:    dw 18
NumberOfHeads:      dw 2

lba_to_chs:
        push ax                    ; Preserve AX
        mov ax, si                 ; Copy LBA to AX
        xor dx, dx                 ; Upper 16-bit of 32-bit value set to 0 for DIV
        div word [SectorsPerTrack] ; 32-bit by 16-bit DIV : LBA / SPT
        mov cl, dl                 ; CL = S = LBA mod SPT
        inc cl                     ; CL = S = (LBA mod SPT) + 1
        xor dx, dx                 ; Upper 16-bit of 32-bit value set to 0 for DIV
        div word [NumberOfHeads]   ; 32-bit by 16-bit DIV : (LBA / SPT) / HEADS
        mov dh, dl                 ; DH = H = (LBA / SPT) mod HEADS
        mov dl, [BOOT_DRIVE]       ; boot device, not necessary to set but convenient
        mov ch, al                 ; CH = C(lower 8 bits) = (LBA / SPT) / HEADS
        shl ah, 6                  ; Store upper 2 bits of 10-bit Cylinder into
        or  cl, ah                 ;     upper 2 bits of Sector (CL)
        pop ax                     ; Restore scratch registers
        ret

;;
;; start of code to execute in second+ boot sector
;;
num_cylinders: dw 0
disk_cx: dw 0
num_drives: db 0
drive_type: db 0
loading_kernel_msg:  db 'Loading kernel... ', 0
align 4
load2:
        mov si, loading_kernel_msg
        call puts16

        ; deterine SectorsPerTrack and Numberofheads
        mov al, [BOOT_DRIVE]
        cmp al, 0x80
        jne load_kernel 	; floppy, chs params already set

	; set up for HDD
	; use int 13/fn 8 to get heads and SPT
	mov ah, 8
	mov dl, [BOOT_DRIVE]
	int 13h

	mov [num_drives], dl
	mov [drive_type], bl
	mov [disk_cx], cx
	xor ah, ah
	mov al, dh
	inc ax
	mov [NumberOfHeads], ax

	mov ax, cx
	and ax, 3fh
	mov [SectorsPerTrack], ax

	; 10011 00 111111	
	mov al, ch
	shr cl, 6
	mov ah, cl
	mov [num_cylinders], ax

global load_kernel
load_kernel:
	push es

	mov ax, [kernel_sector]
	dec ax
	mov cx, [kernel_sectors]
	mov bx, KERNEL_LOAD_SEG
	mov es, bx
	mov si, ax
.loop:
        ; pusha
        ; call .print_loading
        ; popa

	xor bx, bx
	push cx
	push si
	call lba_to_chs
	mov ah, 02h
	int 13h
	pop si
	pop cx
	jc load_error2
	mov bx, es
	add bx, 32
	mov es, bx
	inc si
	loop .loop

	pop es

        mov si, .loaded_msg
        call puts16

        jmp .done

.loaded_msg:        db 'kernel loaded', 13, 10, 0
.chs_msg:           db 'chs: ', 0

align 4
.print_chs:
        pusha
        mov si, .chs_msg
        call puts16
        popa

        pusha
        mov ax, si
        call hexwords16
        popa

        pusha
        mov al, ch
        call hexbytes16
        popa

        pusha
        mov al, dh
        call hexbytes16
        popa

        pusha
        mov al, cl
        call hexbyten16
        popa

        ret

.print_loading:
        ; LBA
        pusha
        mov ax, si
        call hexwords16
        popa

        ; count
        pusha
        mov ax, cx
        call hexwords16
        popa

        ; destination segment
        pusha
        mov ax, es
        call hexword16

        mov al, ':'
        call putc16

        ; destination offset
        popa
        pusha
        mov ax, bx
        call hexwordn16
        popa

        ret

.msg:   db 'loading sector ', 0
.done_msg: db 'Loading completed', 13, 10, 0

.done:
	;                    mov si, .done_msg
	;                    call puts16

        jmp init_graphics


;; query vesa graphics modes
	; CAVEAT PROGRAMMER!  This code overwrites memory_info, so be sure to run this logic before setting up memory_info

struc VesaMode
.attributes         resw 1
.wina               resb 1
.winb               resb 1
.granulatirty       resw 1
.win_size           resw 1
.wina_seg           resw 1
.winb_seg           resw 1
.win_func           resd 1
.pitch              resw 1
.width              resw 1
.height             resw 1
.x_charsize         resb 1
.y_charsize         resb 1
.planes             resb 1
.depth              resb 1
.banks              resb 1
.model              resb 1
.banksize           resb 1
.img_pages          resb 1
.reserved0          resb 1
.red_mask           resb 1
.red_pos            resb 1
.green_mask         resb 1
.green_pos          resb 1
.blue_mask          resb 1
.blue_pos           resb 1
.reserved_mas       resb 1
.reserved_pos       resb 1
.color_attr         resb 1
.fb                 resd 1
.fb_offset          resd 1
.fb_size            resw 1
endstruc

;;;;;;;;;

video_mode_count:    equ video_info
display_mode:        equ video_info + 4

struc VideoMode
.mode               resd 1
.fb                 resd 1
.width              resd 1
.height             resd 1
.pitch              resd 1
.depth              resd 1
endstruc

vesa_mode_msg1:      db 'Getting VESA modes', 13, 10, 0
vesa_mode_msg2:      db '  got mode list', 13, 10, 0
vesa_got_mode_msg:   db '  got mode ', 0
vesa_mode_msgx:      db 'Got VESA modes', 13, 10, 0

vesa_info_block:     equ misc_buffer
vesa_signature:      equ vesa_info_block
vesa_version:        equ vesa_signature + 4
vesa_oem_string:     equ vesa_version + 2
vesa_capabilities:   equ vesa_oem_string + 4
vesa_mode_ptr:       equ vesa_capabilities + 4
vesa_total_memory:   equ vesa_mode_ptr + 4

global init_graphics
init_graphics:
        cli

        mov si, vesa_mode_msg1
        call puts16

        xor ecx, ecx
        mov [video_mode_count], ecx
        mov [display_mode], ecx
        mov es, cx

        mov eax, [.vbe2] 
        mov [vesa_signature], eax

        push es
        push ds
        pop es
        mov ax, 4f00h
        mov di, vesa_info_block
        int 10h
        pop es

        cmp ah, 0
        jz .success

        mov si, .failmsg
        call puts16
        jmp $

.vbe2               db "VBE2"
.failmsg            db 'int 10h/4f00h failed', 13, 10, 0

align 4
.success:
        mov esi, [vesa_mode_ptr]
        test esi, esi
        jz .done

        ; loop through modes, adding mode info to video_info array
        mov edi, video_info + 2 + VideoMode_size
.loop:
        xor eax, eax
        lodsw
        mov [edi + VideoMode.mode], eax

        cmp ax, -1
        je .done

        ; get the info for this mode
        pusha
        mov cx, ax          ; mode
        mov ah, 4fh
        mov al, 1
        mov edi, memory_info
        int 10h
        popa

        ; copy mode info to video_info

        mov eax, [memory_info + VesaMode.fb]
        mov [edi + VideoMode.fb], eax

        xor eax, eax
        mov ax, [memory_info + VesaMode.width]
        mov [edi + VideoMode.width], eax

        mov ax, [memory_info + VesaMode.height]
        mov [edi + VideoMode.height], eax

        mov ax, [memory_info + VesaMode.pitch]
        mov [edi + VideoMode.pitch], eax

        xor eax, eax
        mov al, [memory_info + VesaMode.depth]
        mov [edi + VideoMode.depth], eax

        ; try to choose highest resolution 32 bit display
        mov eax, [edi + VideoMode.depth]
        cmp eax, 32
        jne .next

        mov eax, [display_mode]
        test eax, eax
        je .choose          ; no mode chosen yet, so choose this one

        mov eax, [edi + VideoMode.depth]
        cmp eax, [display_mode + VideoMode.depth]
        jl .next         ; lower depth, skip it

        ; same depth
        mov eax, [edi + VideoMode.height]
        cmp eax, [display_mode + VideoMode.height]
        ja .choose         ; higher resolution, use it
        jne .next           ; lower resolution skip to next

        ; same height
        mov eax, [edi + VideoMode.width]
        cmp eax, [display_mode + VideoMode.width]
        ja .choose         ; same height, higher width
        jne .next           ; lower resolution, so skip it

.choose:
        mov eax, [edi + VideoMode.mode]
        mov [display_mode], ax

        mov eax, [edi + VideoMode.width]
        mov [display_mode + VideoMode.width], eax

        mov eax, [edi + VideoMode.height]
        mov [display_mode + VideoMode.height], eax

        mov eax, [edi + VideoMode.pitch]
        mov [display_mode + VideoMode.pitch], eax

        mov eax, [edi + VideoMode.depth]
        mov [display_mode + VideoMode.depth], eax

        mov eax, [edi + VideoMode.fb]
        mov [display_mode + VideoMode.fb], eax

.next:
        add edi, VideoMode_size
        inc ecx

        jmp .loop
.modemsg            db 'Mode depth ', 0
.done:
        mov [video_mode_count], ecx
        mov long [edi], -1

	;                    mov si, vesa_mode_msgx
	;                    call puts16

%ifdef KGFX
        mov si, gfx_msg
        call puts16

        mov esi, display_mode

        mov eax, [esi + VideoMode.mode]
        call hexwords16

        mov eax, [esi + VideoMode.width]
        call hexwords16

        mov eax, [esi + VideoMode.height]
        call hexwords16

        mov eax, [esi + VideoMode.depth]
        call hexwords16

        mov eax, [esi + VideoMode.fb]
        call hexlongn16

        ; set chosen video display mode
        mov ah, 4fh
        mov al, 02h
        mov ebx, [display_mode + VideoMode.mode]
        or ebx, 1<<14
        int 10h
        jmp probe_memory

	gfx_msg             db '  Setting graphics mode ', 0
%else
        mov si, text_msg
        call puts16
        jmp probe_memory
	text_msg            db 'kernel text mode', 13, 10, 0
%endif

	;
;;; Query memory chunks via BIOS
	;
        ; ebx: which memory block to oget
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
.skip:  clc
        ret
.failed:
	;                    mov si, .failmsg
	;                    call puts16
        stc
        ret
.failmsg: db '  *** get_memory_block failed', 13, 10, 0
.msg:    db '  get_memory_block ', 0

memory_count:        equ memory_info
memory_infos:        equ memory_count + 4

probe_memory_msg:    db 'Probing memory...', 13, 10, 0

probe_memory:
        mov si, probe_memory_msg
        call puts16

        xor ebx, ebx                            ; must be zero to start
        mov [memory_count], ebx
        mov edi, memory_infos
.loop:
        call get_memory_block
        jc .done

        add edi, 24

        mov eax, [memory_count]
        add eax, 1
        mov [memory_count], eax

        jmp .loop

.msg:   db 'Probed memory ', 0

align 4
.done:
        mov si, .msg
        call puts16

enter_32bit_mode:
        cli
        lgdt [gdtr]

        mov eax, cr0
        or al, 1
        mov cr0, eax

        jmp 8:b32

[bits 32]

%include "debug32.inc"
b32msg: db 'Entered 32 bit mode', 13, 10,0

align 16
b32:
        mov esi, b32msg
        call puts32

        mov ax, 16
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        mov ebp, ESP32
        mov esp, ebp

	; copy kernel to final destination
        mov esi, KERNEL_LOAD
        mov edi, KERNEL_ORG
        xor eax, eax
        mov ax, [kernel_sectors]
        mov ecx, 512
        mul ecx
        mov ecx, eax
        rep movsb

	; ; restore ebda_temp at 0x80000
	; mov esi, ebda_temp
	; mov edi, EBDA_ORG
	; mov ecx, 65536 	; 128K
	; rep movsw

	; skip oer GDT, etc.
        jmp enter_long_mode

	
align 4

;;
;; Identity map first 64G of phsical memory
;; See pure64 boot loader on (GitHub https://github.com/ReturnInfinity/Pure64)
;;
identity_map_pure64:
        ; Clear memory for the Page Descriptor Entries (0x10000 - 0x5FFFF)
	mov edi, 0x00010000
	mov ecx, 81920
	rep stosd			; Write 320KiB

        ; Create the Level 4 Page Map. (Maps 4GBs of 2MB pages)
        ; First create a PML4 entry.
        ; PML4 is stored at 0x0000000000002000, create the first entry there
        ; A single PML4 entry can map 512GB with 2MB pages.
	cld
	mov edi, 0x00002000		; Create a PML4 entry for the first 4GB of RAM
	mov eax, 0x00003007		; location of low PDP
	stosd
	xor eax, eax
	stosd

	mov edi, 0x00002800		; Create a PML4 entry for higher half (starting at 0xFFFF800000000000)
	mov eax, 0x00004007		; location of high PDP
	stosd
	xor eax, eax
	stosd

        ; Create the PDP entries.
        ; The first PDP is stored at 0x0000000000003000, create the first entries there
        ; A single PDP entry can map 1GB with 2MB pages
        mov ecx, 4			; number of PDPE's to make.. each PDPE maps 1GB of physical memory
	mov edi, 0x00003000		; location of low PDPE
	mov eax, 0x00010007		; location of first low PD
create_pdpe_low:
	stosd
	push eax
	xor eax, eax
	stosd
	pop eax
	add eax, 0x00001000		; 4K later (512 records x 8 bytes)
	dec ecx
	cmp ecx, 0
	jne create_pdpe_low

        ; Create the low PD entries.
        mov edi, 0x00010000
        mov eax, 0x0000008F		; Bits 0 (P), 1 (R/W), 2 (U/S), 3 (PWT), and 7 (PS) set
        xor ecx, ecx
pd_low:					; Create a 2 MiB page
	stosd
	push eax
	xor eax, eax
	stosd
	pop eax
	add eax, 0x00200000
	inc ecx
	cmp ecx, 2048
	jne pd_low			; Create 2048 2 MiB page maps.

	mov eax, 0x00002000		; Write-thru enabled (Bit 3)
        ret

enter_long_mode:
        mov eax, cr0
        and eax, ~(1<<31)
        mov cr0, eax

        call identity_map_pure64

        ; eax is address of page table (for cr3)

        ; Enable extended properties
        push eax
	mov eax, cr4
	or eax, 0x0000000B0		; PGE (Bit 7), PAE (Bit 5), and PSE (Bit 4)
	mov cr4, eax
        pop eax

        ; Point cr3 at PML4
	mov cr3, eax

        ; Enable long mode and SYSCALL/SYSRET
	mov ecx, 0xC0000080		; EFER MSR number
	rdmsr				; Read EFER
	or eax, 0x00000101 		; LME (Bit 8)
	wrmsr				; Write EFER

        ; Enable paging to activate long mode
	mov eax, cr0
	or eax, 0x80000000		; PG (Bit 31)
	mov cr0, eax


        lgdt [gdtr64]
        jmp CODE_SEG:go64


[bits 64]
%include "debug64.inc"

	boot64msg           db 'Entered long mode', 13, 10, 0
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

        mov rsi, boot64msg
        call puts64


global call_main
call_main:

	; set up SYSINFO at sys_info
	mov rdi, sys_info

	xor rax, rax

	mov [rdi + SYSINFO.milliseconds], rax

	mov al, [bochs_present]
	mov [rdi + SYSINFO.bochs], rax

	mov al, [num_drives]
	mov [rdi + SYSINFO.num_drives],rax

	xor rax, rax
	mov ax, [disk_cx]
	mov [rdi + SYSINFO.disk_cx], ax
	
	xor rax, rax
	mov al, [BOOT_DRIVE]
	mov [rdi + SYSINFO.boot_drive], rax
	
	xor rax, rax
	mov ax, [SectorsPerTrack]
	inc ax
	mov [rdi + SYSINFO.sectors_per_track], ax

	xor rax, rax
	mov ax, [NumberOfHeads]
	mov [rdi + SYSINFO.num_heads], rax

	xor rax, rax
	mov ax, [num_cylinders]
	inc ax
	mov [rdi + SYSINFO.num_cylinders], rax

	mov ax, [boot_sector]
	mov [rdi + SYSINFO.boot_sector], rax
	mov ax, [boot_sectors]
	mov [rdi + SYSINFO.boot_sectors], rax

	mov ax, [kernel_sector]
	mov [rdi + SYSINFO.kernel_sector], rax
	mov ax, [kernel_sectors]
	mov [rdi + SYSINFO.kernel_sectors], rax

	mov ax, [root_sector]
	mov [rdi + SYSINFO.root_sector], rax

	mov eax, [display_mode + VideoMode.mode]
	mov [rdi + SYSINFO.video_mode], rax

	mov eax, [display_mode + VideoMode.width]
	mov [rdi + SYSINFO.screen_width], rax

	mov eax, [display_mode + VideoMode.height]
	mov [rdi + SYSINFO.screen_height], rax

	mov eax, [display_mode + VideoMode.depth]
	mov [rdi + SYSINFO.screen_depth], rax

	mov eax, [display_mode + VideoMode.pitch]
	mov [rdi + SYSINFO.screen_pitch], rax

        mov eax, [display_mode + VideoMode.fb]
        mov [rdi + SYSINFO.framebuffer], eax

	xor rax, rax
	mov ax, [ebda]
	shl rax, 4
	mov [rdi + SYSINFO.ebda], rax

	push rax
	push rdi

	mov rax, 0xffffff
	mov rcx, 2560 * 1600
	xor rdi, rdi
        mov edi, [display_mode + VideoMode.fb]
	rep stosd

	pop rdi
	pop rax
	
	; call kernel with SYSINFO as argument
	xor ah, ah
	mov al, [bochs_present]
        call KERNEL_ORG
        cli
        jmp $

ap_start64:
	xor rax, rax
	xor rbx, rbx
	xor rcx, rcx
	xor rdx, rdx
	xor rsi, rsi
	xor rdi, rdi
	xor rbp, rbp
	xor r8, r8
	xor r9, r9
	xor r10, r10
	xor r11, r11
	xor r12, r12
	xor r13, r13
	xor r14, r14
	xor r15, r15

	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax

	; NOP out the initial jmp instruction
	; mov al, 0x90		;NOP instruction
	; mov [BOOTSTRAP_ORG+0], al
	; mov [BOOTSTRAP_ORG+1], al
	; mov [BOOTSTRAP_ORG+2], al
	; mov [BOOTSTRAP_ORG+3], al
	; mov [BOOTSTRAP_ORG+4], al

	mov rdi, [sys_info]
	mov ah, [CPU_NUM]
	mov al, [bochs_present]
	call KERNEL_ORG
	cli
	jmp $
	
.call_message:
	db 'calling KERNEL_ORG ', 0

