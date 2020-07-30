%macro BOCHS 0
	xchg bx,bx
%endmacro
	
%macro bochs 0
	xchg bx,bx
%endmacro

;; rdi = destination
;; rsi = src
;; rdx = width
;; rcx = height
;; r8 = dst pitch
;; r9 = src pitch
global CopyRect
CopyRect:
	push rcx 		; save height
	mov rcx, rdx		; load width

	push rsi		; save src
	push rdi		; save dst

	rep movsd		; copy row of pixels

	pop rdi			; restore dst
	add rdi, r8		; next row

	pop rsi			; restore src
	add rsi, r9		; next row

	pop rcx			; restore height
	loop CopyRect

	ret

;; CopyRGB(dst, src, count)
global CopyRGB
CopyRGB:
	push rcx
	mov rcx, rdx
	rep movsd
	pop rcx
	ret

;; CopyRGB64(dst, src, count)
;; copy RGB pixels, two pixels at a time
global CopyRGB64
CopyRGB64:
	push rcx
	mov rcx, rdx
	rep movsq
	pop rcx
	ret
