;; screen.inc

	%macro puts 1
	push eax
	mov esi, %%puts_str
	call screen_puts
	jmp %%puts_end
%%puts_str:
	db %1
	db 0
%%puts_end:
	pop eax
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
                    push es
                    pusha
                    mov cx, 25*80
                    mov edi, 0xb8000

                    mov ah, WHITE_ON_GREEN
                    mov al, 'x'
                    rep stosw

                    xor ax,ax
                    mov [screen_row],ax
                    mov [screen_col],ax
                    call screen_moveto
                    
                    popa
                    pop es
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
                    pusha
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
                    popa
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
