%if 0
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
                  
                    xor ax, ax
                    mov es, ax

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
                    call newline16
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
                   
%endif
