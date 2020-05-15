                    [bits 64]

                    align 8
;screen:             dq VIDEO_MEMORY
nybbles             db "0123456789ABCDEF",0

                    align 8

                    global debug64_init
debug64_init:
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
;cls64:
;                    mov rdi, 0xb8000
;                    mov ah, WHITE_ON_BLACK
;                    mov al, '-'
;                    mov rcx, 25*80
;                    rep stosw
;                    ret

                    global putc64
putc64:
   %ifdef SERIAL
                    push rdx
                    push rax
                    mov dx, COM1+5
.wait:              
                    in al, dx
                    and al, 0x20
                    jz .wait
                    pop rax
                    mov dx, COM1
                    out dx, al
                    pop rdx
   %else
;                    call screen_putc
                    out 0xe9, al
;                    xor bx,bx
;                    mov ah, 0eh
;                    int 10h			; Otherwise, print it
   %endif
	ret

                    global puts64
puts64:			; Routine: output string in SI to screen
	lodsb			; Get character from string
	cmp al, 0
	je .done		; If char is zero, end of string
                    push rdi
	call putc64
                    pop rdi
	jmp puts64

.done:
	ret

                    global newline64
newline64:
	push rax
;                    mov al, 13
;                    call screen_putc
	mov al, 10
	call putc64
	pop rax
	ret

                    global space64
space64:
	mov al, ' '
	jmp putc64

nybbles64:
	db "0123456789ABCDEF"

hexnybble64:
;                    push rbx
	and rax, 0x0f
;                    xor rbx, rbx
;                    mov rbx, rax
	mov al, [nybbles64+rax]
	call  putc64
;                    pop rbx
	ret

                    global hexbyte64
hexbyte64:
	push rax
	push rcx

	push rax
	ror rax, 4
	call hexnybble64
	pop rax
	call hexnybble64

	pop rcx
	pop rax
	ret

                    global hexword64
hexword64:
	push rax
	push rcx
	push rax
	ror rax, 8
	and rax, 0xff
	call hexbyte64
	pop rax
	and rax, 0xff
	call hexbyte64
	pop rcx
	pop rax
	ret

                    global  hexlong64
hexlong64:
	push rax
	push rcx
	push rax
	ror rax, 16
	and rax, 0xffff
	call hexword64
	pop rax
	and rax, 0xffff
	call hexword64
	pop rcx
	pop rax
	ret

                    global  hexquad64
hexquad64:
                    push rax
                    push rcx
                    push rax
                    ror rax, 32
                    mov rcx, 0xffffffff
                    and rax, rcx
                    call hexlong64
                    pop rax
                    mov rcx, 0xffffffff
                    and rax, rcx
                    call hexlong64
                    pop rcx
                    pop rax
                    ret

                    global hexbytes64
hexbytes64:         call hexbyte64
                    jmp space64

                    global hexwords64
hexwords64:         call hexword64
                    jmp space64

                    global hexlongs64
hexlongs64:         call hexlong64
                    jmp space64

                    global hexquads64
hexquads64:         call hexquad64
                    jmp space64

                    global hexbyten64
hexbyten64:         call hexbyte64
                    jmp newline64

                    global hexwordn64
hexwordn64:         call hexword64
                    jmp newline64

                    global hexlongn64
hexlongn64:         call hexlong64
                    jmp newline64

                    global hexquadn64
hexquadn64:         call hexquad64
                    jmp newline64

                    global hexdump64
hexdump64:
                    mov rax, rsi
                    call hexquads64
.loop:
	lodsb
	call hexbytes64
	dec rcx
	ja .loop
                    call newline64
	ret

                    global stackdump64
stackdump64:
                    push rsi
                    mov rsi, rsp
                    add rsi, 16

                    push rax
                    push rbx
                    push rcx

                    call dump1
                    call dump1
                    call dump1
                    call dump1
                    call dump1
                    call dump1
                    call dump1
                    call dump1
                    call dump1
                    call dump1
                    call dump1
                    call dump1
                    call dump1

                    call newline64

                    pop rcx
                    pop rbx
                    pop rax
                    pop rsi
                    ret

dump1:
                    push rsi
                    mov rax, rsi
                    call hexquad64
                    mov al, ' '
                    call putc64
                    pop rsi
                    lodsq
                    call hexquad64
                    call newline64
                    ret

