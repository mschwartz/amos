;; debug16.inc

global debug16_init
debug16_init:
	mov bl, [bochs_present]
	test bl, bl
	jne .bochs

        mov al, 0x00
        mov dx, COM1+1
        out dx, al

        mov al, 0x80
        mov dx, COM1+3
        out dx, al

        mov al, 0x03
        mov dx, COM1+0
        out dx, al

        mov al, 0x00
        mov dx, COM1+1
        out dx, al

        mov al, 0x03
        mov dx, COM1+3
        out dx, al

        mov al, 0xc7
        mov dx, COM1+2
        out dx, al

        mov al, 0x0b
        mov dx, COM1+4
        out dx, al
        
.bochs:
        ret

%if 0
cls16:
        mov di, 0
        push VIDEO_MEMORY_SEG
        pop es
        mov ah, WHITE_ON_BLACK
        mov al, '-'
        mov ecx, 25*80
        rep stosw
        ret
%endif
putc16:
        push dx
	mov dl, [bochs_present]
	test dl, dl
	jne .bochs
	
        push ax
        mov dx, COM1+5
        ; wait for serial ready to send
.wait:              
        in al, dx
        and al, 0x20
        jz .wait

        pop ax
        mov dx, COM1
        out dx, al
        pop dx

	ret
.bochs:
        out 0xe9, al
	xor bx,bx
	mov ah, 0eh
	int 10h			; Otherwise, print it
	pop dx
	ret

puts16:			; Routine: output string in SI to screen
        pusha
.loop:
	lodsb			; Get character from string
	cmp al, 0
	je .done		; If char is zero, end of string
	call putc16
	jmp .loop
.done:
        popa
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
	pusha
	mov al, ' '
	call putc16
	popa
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
        pusha
        mov ax, si
        call hexword16
        call space16
        popa
.loop:
	lodsb
	call hexbyte16
        call space16
	dec cx
	ja .loop
        call newline16
	ret

%if 0
hexdump16x:
.line:
        pusha
        mov eax, esi
        call hexlong16
        call space16
        popa
	mov bx, 8
.loop:
	lodsb
	call hexbyte16
	dec bx
	jne .space
	call newline16
	mov bx, 8
	jmp .line
	; jmp .cont
.space:
        call space16
.cont:
	dec ecx
	ja .loop
        call newline16
	ret
%endif

hexbytes16:
	call hexbyte16
        jmp space16
hexwords16:
	call hexword16
        jmp space16
hexlongs16:
	call hexlong16
        jmp space16
hexbyten16:
	call hexbyte16
        jmp newline16
hexwordn16:
	call hexword16
        jmp newline16
hexlongn16:
	call hexlong16
        jmp newline16
