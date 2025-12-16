
; *** panic/panic.inc ***

; >> void panic() <<

; Unrecoverable error handler.
; Print caller address and halt.

panic:
	clr	ea	; disable interrupts
	mov	r0,a
	mov	dptr,#panic_out
	mov	out,dpl
	mov	out+1,dph
	mov	dptr,#panic_text
	acall	print_text
	pop	acc
	acall	print_hex_8
	pop	acc
	acall	print_hex_8
	mov	a,#' '
	acall	print_char
	mov	a,r0
	acall	print_hex_8
	mov	a,#13
	acall	print_char
	mov	a,#10
	acall	print_char
	sjmp	*

panic_text:
	.db	13, 10
	.db	" !!! panic @"
	.db	0
