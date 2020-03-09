                    [bits 64]

VIDEO_MEMORY        equ 0xb8000
                    align 8
screen:             dq VIDEO_MEMORY
nybbles             db "0123456789ABCDEF",0

                    align 8

putc:
                    push rdx

                    mov rdx, [screen]
                    mov ah, WHITE_ON_BLACK
                    mov [rdx], ax
                    add rdx, 2
                    mov [screen], rdx

                    pop rdx
                    ret

                    ret
                   
;newline:
;                    mov al, 13
;                    call screen_putc
;                    mov al, 10
;                    call screen_putc
;                    ret

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

hexbyte:
                    push rax
	ror rax, 4
	call hexnybble
                    pop rax
                    call hexnybble

	ret

hexword:
                    push rax
	ror rax, 8
	and rax, 0xff
                    call hexbyte
                    pop rax
	and rax, 0xff
                    call hexbyte
	ret

hexlong:            
                    push rax
                    ror rax, 16
                    and rax, 0xffff
                    call hexword
                    pop rax
                    call hexword
	ret

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

