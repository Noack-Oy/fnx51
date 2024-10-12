
; *** print/text.inc ***

; >> void print_text(char* dptr) <<

; Print null-terminated string from program code.
; Aborts if no null terminator is found after 256 bytes.

print_text:
	push	ACC
	mov	a,	r0
	push	ACC

	clr	a
	mov	r0,	a
print_text__loop:
	movc	a,	@dptr+a
	jz	print_text__end
	acall	print_char
	inc	r0
	mov	a,	r0
	jnz	print_text__loop

print_text__end:
	pop	ACC
	mov	r0,	a
	pop	ACC
	ret
