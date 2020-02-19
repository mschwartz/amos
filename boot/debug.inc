VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f

                    [bits 64]
                    align 8
screen:             dq VIDEO_MEMORY
                    align 8

putc:
;                    push rdx

                    mov r8, [screen]
                    mov ah, WHITE_ON_BLACK
                    mov [r8], ax
                    add r8, 2
                    mov [screen], r8

;                    pop rdx
                    ret
                   
;newline:
;                    push eax
;                    mov al, 13
;                    call screen_putc
;                    mov al, 10
;                    call screen_putc
;                    pop eax
;                    ret

space:
	push rax
	mov al, ' '
	call putc
	pop rax
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

nybbles:
	db "0123456789ABCDEF"

                    align 4

hexnybble:
                    push rbx

	and al, 0x0f
                    xor rbx, rbx
                    mov bl, al
                    add rbx, nybbles
                    mov al, [rbx]
                    call putc

                    pop rbx
	ret

hexbyte:
;                    push rax
                    call hexnybble
;                    pop rax
                    ret
;                    push rax
	ror rax, 4
	call hexnybble
;                    pop rax
;                    call hexnybble

	pop rax
	ret

hexword:
	push rax
	push rax
	ror rax, 8
	and rax, 0xff
	call hexbyte
	pop rax
	and rax, 0xff
	call hexbyte
	pop rax
	ret

hexlong:            
	push rax
	push rax
                    ror eax, 16
                    and rax, 0xffff
                    call hexword
	pop rax
                    and eax, 0xffff
                    call hexword
	pop rax
	ret

hexdump:

                    mov rax, rsi
                    call hexbyte
;                    call hexlong
                    ret
                    call space
hexdump_loop:
	lodsb
	call hexbyte
	mov al, ' '
	call putc
                    loop hexdump_loop
	ret

