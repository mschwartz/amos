                    [bits 64]

VIDEO_MEMORY        equ 0xb8000
                    align 8
screen:             dq VIDEO_MEMORY
nybbles             db "0123456789ABCDEF",0

                    align 8

putc:
                    push dx
                    mov dx, 3f9h
                    out dx, al
                    pop dx
                    ret

newline64:
                    mov al, 13
                    call putc
                    mov al, 10
                    call putc
                    ret

space:
	mov al, ' '
	call putc
	ret
print32:
.loop:
                    mov al, [rbx]
                    cmp al, 0
                    je .done
                    call putc
                    add rbx, 1
                    jmp .loop
.done:
                    ret

hexnybble:
                    and al, 0x0f
                    xor rbx, rbx
                    mov bl, al
                    add rbx, nybbles
                    mov al, [rbx]
                    call putc

	ret

                    global hexbyte
hexbyte:
                    push rax
	ror rax, 4
	call hexnybble
                    pop rax
                    call hexnybble

	ret

                    global hexword
hexword:
                    push rax
	ror rax, 8
	and rax, 0xff
                    call hexbyte
                    pop rax
	and rax, 0xff
                    call hexbyte
	ret

                    global hexlong
hexlong:            
                    push rax
                    ror rax, 16
                    and rax, 0xffff
                    call hexword
                    pop rax
                    call hexword
	ret

                    global hexquad
hexquad:            
                    push rbx
                    push rax
                    mov rbx, 0xffffffff
                    ror rax, 32
                    and rax, rbx
                    call hexlong
                    pop rax
                    call hexlong
                    pop rbx
	ret

                    global hexdump
hexdump:
                    mov rax, rsi
                    call hexword
                    mov al, '-'
                    call putc

hexdump_loop:
	lodsb
	call hexbyte
	mov al, ' '
	call putc
                    loop hexdump_loop
	ret

                    global stackdump
stackdump:
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
                    call hexquad
                    mov al, ' '
                    call putc
                    pop rsi
                    lodsq
                    call hexquad
                    call newline64
                    ret

