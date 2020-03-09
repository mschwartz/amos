;; debug16.inc

%if 0
	%macro puts16 1
	push eax
	mov esi, %%puts_str
	call puts16
	jmp %%puts_end
%%puts_str:
	db %1
	db 0
%%puts_end:
	pop eax
	%endmacro
                    %endif
cls16:
                    mov di, 0
                    push VIDEO_MEMORY_SEG
                    pop es
                    mov ah, WHITE_ON_BLACK
                    mov al, '-'
                    mov ecx, 25*80
                    rep stosw
                    ret
putc16:
	xor bx,bx
	mov ah, 0eh
	int 10h			; Otherwise, print it
	ret

puts16:			; Routine: output string in SI to screen
	lodsb			; Get character from string
	cmp al, 0
	je .done		; If char is zero, end of string
                    push di
	call putc16
                    pop di
	jmp puts16

.done:
	ret

newline16:
	push eax
;                    mov al, 13
;                    call screen_putc
	mov al, 10
	call putc16
	pop eax
	ret

space16:
	push ax
	mov al, ' '
	call putc16
	pop ax
	ret

nybbles16:
	db "0123456789ABCDEF"

hexnybble16:
	push bx
	and ax, 0x0f
	mov bx, ax
	mov al, [nybbles16+bx]
	call  putc16
	pop bx
	ret

hexbyte16:
	push eax
	push ecx

	push eax
	ror ax, 4
	call hexnybble16
	pop eax
	call hexnybble16

	pop ecx
	pop eax
	ret

hexword16:
	push eax
	push ecx
	push eax
	ror eax, 8
	and eax, 0xff
	call hexbyte16
	pop eax
	and eax, 0xff
	call hexbyte16
	pop ecx
	pop eax
	ret

hexlong16:
	push eax
	push ecx
	push eax
	ror eax, 16
	and eax, 0xffff
	call hexword16
	pop eax
	and eax, 0xffff
	call hexword16
	pop ecx
	pop eax
	ret

hexdump16:
;                    push cx
;                    push si
;                    mov ax, si
;                    call hexword16
;                    call space16
;                    pop si
;                    pop cx
hexdump_loop16:
	lodsb
	call hexbyte
	mov al, ' '
	call putc16
	dec cx
	ja hexdump_loop16
	ret

