;; screen.inc

	%macro puts 1
	push rax
	mov esi, %%puts_str
	call screen_puts
	jmp %%puts_end
%%puts_str:
	db %1
	db 0
%%puts_end:
	pop rax
	%endmacro
       
WHITE_ON_BLACK equ 0x0f
WHITE_ON_GREEN equ 0x1e

VGA_WIDTH equ 80

screen_row:         dw 0
screen_col:         dw 0

; address cursor
screen_moveto:
                    ;  bx = x, ax = y
                    mov ax, [screen_row]
                    mov bx, [screen_col]
                    mov dl, VGA_WIDTH
                    mul dl
                    add bx, ax

                    mov dx, 03d4h
                    mov al, 0x0f
                    out dx, al

                    inc dl
                    mov al, bl
                    out dx, al

                    dec dl
                    mov al, 0x0e
                    out dx, al

                    inc dl
                    mov al, bh
                    out dx, al
                    ret
                    
; clear screen
screen_clear:
;                    push es
                    push rax
                    push rcx
                    push rdi

                    mov rcx, 25*80
                    mov rdi, 0xb8000

                    mov ah, WHITE_ON_GREEN
;                    mov al, 'x'
                    rep stosw

                    xor rax, rax
                    mov [screen_row],ax
                    mov [screen_col],ax
                    call screen_moveto
                   
                    pop rdi
                    pop rcx
                    pop rax
                    ret

; output character to screen
screen_putc:
                    cmp ax, 10
                    jne screen_putc_character
                    xor ax, ax
                    mov [screen_col], ax
                    mov ax, [screen_row]
                    add ax, 1
                    mov [screen_row], ax
                    jmp screen_moveto
                    ret

screen_putc_character:
                    push rax
                    push rbx
                    push rcx
                    push rdx

                    mov edx, 0b8000h

                    xor ebx, ebx
                    mov bx, [screen_row]
                    add ebx, ebx
                    imul ebx, VGA_WIDTH
                    add edx, ebx
                    xor ebx, ebx
                    mov bx, [screen_col]
                    add ebx, ebx
                    add edx, ebx
                    mov ah, WHITE_ON_BLACK
                    mov [edx], ax

                    mov ax, [screen_col]
                    inc ax
                    cmp ax, 79
                    jb screen_putc_advanced
                    xor ax,ax

screen_putc_advanced:
                    mov [screen_col], ax
                    call screen_moveto
                    pop rdx
                    pop rcx
                    pop rbx
                    pop rax
                    ret

screen_puts:			; Routine: output string in SI to screen
.repeat:
	lodsb		; Get character from string
	cmp al, 0
	je .done		; If char is zero, end of string

                    cmp al, '\'
                    jne .normal
                    lodsb
                    cmp al, 'n'
                    je .newline
                    push ax
                    mov al, '\'
                    call screen_putc
                    pop ax
                    jmp .normal
.newline:
                    mov ax, 10

.normal:
	call screen_putc
	jmp .repeat

.done:
	ret

screen_space:
                    mov al, ' '
                    jmp screen_putc
screen_newline:
                    mov al, 10
                    jmp screen_newline

screen_nybbles:
	db "0123456789ABCDEF"

screen_hexnybble:
;                    push rbx
	and rax, 0x0f
;                    xor rbx, rbx
;                    mov rbx, rax
	mov al, [screen_nybbles+rax]
	call  screen_putc
;                    pop rbx
	ret

screen_hexbyte:
	push rax
	push rcx

	push rax
	ror rax, 4
	call screen_hexnybble
	pop rax
	call screen_hexnybble

	pop rcx
	pop rax
	ret

screen_hexword:
	push rax
	push rcx
	push rax
	ror rax, 8
	and rax, 0xff
	call screen_hexbyte
	pop rax
	and rax, 0xff
	call screen_hexbyte
	pop rcx
	pop rax
	ret

screen_hexlong:
	push rax
	push rcx
	push rax
	ror rax, 16
	and rax, 0xffff
	call screen_hexword
	pop rax
	and rax, 0xffff
	call screen_hexword
	pop rcx
	pop rax
	ret

screen_hexquad:
	push rax
	push rcx
	push rax
	ror rax, 32
	and rax, 0xffffffff
	call screen_hexlong
	pop rax
	and rax, 0xffffffff
	call screen_hexlong
	pop rcx
	pop rax
	ret

screen_hexdump:
;                    push cx
;                    push si
;                    mov ax, si
;                    call hexword
;                    call space
;                    pop si
;                    pop cx
                    mov rax, rsi
                    call screen_hexquad
                    call screen_space
.loop:
	lodsb
	call screen_hexbyte
                    call screen_space
	dec rcx
	ja .loop
                    call screen_newline
	ret

screen_hexbytes:    call screen_hexbyte
                    jmp screen_space
screen_hexwords:    call screen_hexword
                    jmp screen_space
screen_hexlongs:    call screen_hexlong
                    jmp screen_space
screen_hexbyten:    call screen_hexbyte
                    jmp screen_newline
screen_hexwordn:    call screen_hexword
                    jmp screen_newline
screen_hexlongn:    call screen_hexlong
                    jmp screen_newline
