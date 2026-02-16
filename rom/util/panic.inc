
; *** panic/panic.inc ***

; >> void panic() <<

; Unrecoverable error handler.
; Print caller address and halt.

panic:
	clr	ea	; disable interrupts
	mov	r0,a	; save a
	; display panic text
	mov	dptr,#panic_out
	mov	out,dpl
	mov	out+1,dph
	mov	dptr,#panic_text
	acall	print_text
	; get return address from stack
	pop	acc
	mov	r2,a
	pop	acc
	; subtract two to get call instruction address
	clr	c
	subb	a,#2
	mov	r1,a
	mov	a,r2
	subb	a,#0
	; print caller address (to look up cause in .lst file)
	acall	print_hex_8
	mov	a,r1
	acall	print_hex_8
	; print original value of a (error code / debug info)
	mov	a,#' '
	acall	print_char
	mov	a,r0
	acall	print_hex_8
	; newline
	mov	a,#13
	acall	print_char
	mov	a,#10
	acall	print_char
	; hang
	sjmp	*

panic_text:
	.db	13, 10
	.db	" !!! panic @"
	.db	0
