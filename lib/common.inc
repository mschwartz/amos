;; common.inc

putchar:
	xor bx,bx
	mov ah, 0eh
	int 10h			; Otherwise, print it
	ret

print_string:			; Routine: output string in SI to screen

.repeat:
	lodsb			; Get character from string
	cmp al, 0
	je .done		; If char is zero, end of string
	call screen_putc
	jmp .repeat

.done:
	ret

newline:
	push eax
;                    mov al, 13
;                    call screen_putc
	mov al, 10
	call screen_putc
	pop eax
	ret

space:
	push ax
	mov al, ' '
	call screen_putc
	pop ax
	ret

nybbles:
	db "0123456789ABCDEF"

hexnybble:
	push bx
	and ax, 0x0f
	mov bx, ax
	mov al, [nybbles+bx]
	call  screen_putc
	pop bx
	ret

hexbyte:
	push eax
	push ecx

	push eax
	ror ax, 4
	call hexnybble
	pop eax
	call hexnybble

	pop ecx
	pop eax
	ret

hexword:
	push eax
	push ecx
	push eax
	ror eax, 8
	and eax, 0xff
	call hexbyte
	pop eax
	and eax, 0xff
	call hexbyte
	pop ecx
	pop eax
	ret

hexlong:            
	push eax
	push ecx
	push eax
	ror eax, 16
	and eax, 0xffff
	call hexword
	pop eax
	and eax, 0xffff
	call hexword
	pop ecx
	pop eax
	ret

hexdump:
                    mov ax, si
                    call hexword
                    call space
hexdump_loop:
	lodsb
	call hexbyte
	mov al, ' '
	call screen_putc
	dec cx
	ja hexdump_loop
	ret

