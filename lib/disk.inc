;; disk.inc

di_drive_number equ 0
di_valid equ di_drive_number + 1
di_num_cylinders equ di_valid + 1
di_num_drives equ di_num_cylinders + 2
di_num_heads equ di_num_drives + 1
di_num_sectors equ di_num_heads + 1
di_pad equ di_num_sectors + 1
di_sizeof equ di_pad + 1

;
;;; Get information about a drive
;;;
;;; input:
;;;   si = address of drive_info struct to be filled in
;;;   dl = drive #
                    global get_drive_info
get_drive_info:
                    push si


                    ; set struct to all zeros
                    or eax,eax
                    mov [si + 0], eax
                    mov [si + 4], eax
                    mov [si + di_drive_number], dl

                    ; clear pointer to work around certain buggy BIOS
                    mov es, ax
                    mov di, ax

                    mov ah, 0x08
                    int 0x13
                    jc get_drive_info_fail
                    pop si              ; disk_info structure address

                    push ax             ; save return code
                    mov [si + di_num_drives], dl
                    inc dh
                    mov [si + di_num_heads], dh
                    push cx
                    shr cx, 5
                    inc cx
                    mov [si + di_num_cylinders], cx
                    pop cx
                    and cx, 11111b
                    mov ax, cx
;                    call hexword

                    mov [si + di_num_sectors], cl

                    ; structure data is valid!
                    mov ax, -1
                    mov [si + di_valid], al
                    pop ax
                    ret
get_drive_info_fail:
                    puts "FAIL"
                    pop si
                    ret

;
;;; calculate absolute sector, head, and track for a given disk 
;;; outputs in registers suitable for int 13/02 = read sectors from drive
;;;
;;; inputs:
;;;   si = drive_info struct address
;;;   ax = logical sector number (LBA)
;;; outputs:
;;;   ch = cylindar
;;;   cl = sector
;;;   dh = head
;;;   dl = drive
;;;
;;; See: http://www.osdever.net/tutorials/view/lba-to-chs
;
sector_address:
                    push esi
                    xor dx,dx		; Zero dx
                    mov bx, [si+di_num_sectors]	; Move into place STP (LBA all ready in place)
                    div bx		; Make the divide (ax/bx -> ax,dx)
                    inc dx		; Add one to the remainder (sector value)
                    push dx		; Save the sector value on the stack

                    xor dx,dx		; Zero dx
                    mov bx, [si+di_num_heads]	; Move NumHeads into place (NumTracks all ready in place)
                    div bx		; Make the divide (ax/bx -> ax,dx)

                    mov cx,ax		; Move ax to cx (Cylinder)
                    mov bx,dx		; Move dx to bx (Head)
                    pop ax                                 	; Take the last value entered on the stack off.             
                                                            ; It doesn't need to go into the same register.
                                                            ; (Sector)
                    pop esi
                    mov dx, cx
                    xor cx, cx
                    mov ch, dl                              ; cylindar
                    mov cl, al                              ; sector
                    mov dh, bl                              ; head
                    mov dl, [si+di_drive_number]
                    ret		; Return to the main function


; 
;;; Load sector(s) from drive 
;;
;;; inputs:
;;;   si: address of drive_info struct to read from
;;;   di: address to read sector to
;;;   ax: lba sector number to read
;;;
;;; outputs:
;;;   sector(s) read in to memory at es:di
;;;   ax: number of sectors read
;;;
;
                    global load_sectors
load_sectors:
                    call sector_address
	mov ah, 0x02
	int 0x13
	xor ah,ah
	ret

;
;;; reset a disk drive
;;;
;;; inputs:
;;;   si: disk_info struct of disk to reset
                    global reset_disk
reset_disk:
;                    puts "Resetting disk #"
;                    xor ax,ax
;                    mov al, [bootDrive]
;                    call hexbyte

                    mov dl, [si + di_drive_number]
                    xor ax, ax
                    int 0x13
;                    call newline
                    ret

