;; debug32.inc

%define SERIAL
%if 0
	%macro puts32 1
	push eax
	mov esi, %%puts_str
	call puts32
	jmp %%puts_end
%%puts_str:
	db %1
	db 0
%%puts_end:
	pop eax
	%endmacro
%endif

COM1                equ 0x3f8
                    global debug32_init
debug32_init:
                    %ifdef SERIAL
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
                    
                    %endif
                    ret

cls32:
                    mov di, 0
                    push VIDEO_MEMORY_SEG
                    pop es
                    mov ah, WHITE_ON_BLACK
                    mov al, '-'
                    mov ecx, 25*80
                    rep stosw
                    ret
putc32:
                    %ifdef SERIAL
                    push dx
                    push ax
                    mov dx, COM1+5
.wait:              
                    in al, dx
                    and al, 0x20
                    jz .wait
                    pop ax
                    mov dx, COM1
                    out dx, al
                    pop dx
                    %else
                    out 0xe9, al
	xor bx,bx
	mov ah, 0eh
	int 10h			; Otherwise, print it
                    %endif
	ret

puts32:			; Routine: output string in SI to screen
	lodsb			; Get character from string
	cmp al, 0
	je .done		; If char is zero, end of string
                    push di
	call putc32
                    pop di
	jmp puts32

.done:
	ret

newline32:
	push eax
;                    mov al, 13
;                    call screen_putc
	mov al, 10
	call putc32
	pop eax
	ret

space32:
	pusha
	mov al, ' '
	call putc32
	popa
	ret

nybbles32:
	db "0123456789ABCDEF"

hexnybble32:
	push bx
	and ax, 0x0f
	mov bx, ax
	mov al, [nybbles32+bx]
	call  putc32
	pop bx
	ret

hexbyte32:
	push eax
	push ecx

	push eax
	ror ax, 4
	call hexnybble32
	pop eax
	call hexnybble32

	pop ecx
	pop eax
	ret

hexword32:
	push eax
	push ecx
	push eax
	ror eax, 8
	and eax, 0xff
	call hexbyte32
	pop eax
	and eax, 0xff
	call hexbyte32
	pop ecx
	pop eax
	ret

hexlong32:
	push eax
	push ecx
	push eax
	ror eax, 16
	and eax, 0xffff
	call hexword32
	pop eax
	and eax, 0xffff
	call hexword32
	pop ecx
	pop eax
	ret

hexdump32:
;                    push cx
;                    push si
;                    mov ax, si
;                    call hexword32
;                    call space32
;                    pop si
;                    pop cx
                    pusha
                    mov ax, si
                    call hexword32
                    call space32
                    popa
.loop:
	lodsb
	call hexbyte32
                    call space32
	dec cx
	ja .loop
                    call newline32
	ret

hexbytes32:         call hexbyte32
                    jmp space32
hexwords32:         call hexword32
                    jmp space32
hexlongs32:         call hexlong32
                    jmp space32
hexbyten32:         call hexbyte32
                    jmp newline32
hexwordn32:         call hexword32
                    jmp newline32
hexlongn32:         call hexlong32
                    jmp newline32
