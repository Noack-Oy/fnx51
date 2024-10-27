
; *** util/dump.inc ***

; >> void dump(uint32 &r0r1r2r3, uint8 a) <<

; Print hexdump from standard input to standard output.
; Starting value for address column in r0-r3.
; Number of bytes to print in a (0 means 256).
; r0-r4 contains next address upon exit.

dump:
	push	b
	push	acc
	mov	b,	a
	mov	a,	r4
	push	acc
	mov	a,	r5
	push	acc
	mov	a,	r6
	push	acc
	mov	a,	r7
	push	acc

	; b: total bytes remaining
	; r0: buffer pointer
	; r1: read loop counter / padding size
	; r2: print loop counter

	; r4-7: address label
	acall	xch_r0123_r4567

	; allocate 16 byte array on stack
	mov	a,	sp
	mov	r0,	a
	add	a,	#16
	mov	sp,	a

dump__line_loop:
	; read into buffer
	mov	r1,	#16
dump__read_loop:
	acall	read_char
	inc	r0
	mov	@r0,	a
	djnz	b,	dump__read_continue
	dec	r1
	sjmp	dump__read_end
dump__read_continue:
	djnz	r1,	dump__read_loop
dump__read_end:

	; print address column
	acall	xch_r0123_r4567
	acall	print_hex_32
	acall	xch_r0123_r4567
	mov	a,	#':'
	acall	print_char

	; if r1 is nonzero here, we have a shorter line
	; the value in r1 is the number of padding bytes

	; wind back pointer
	mov	a,	r0
	add	a,	#-16
	add	a,	r1
	mov	r0,	a

	; print hex
	mov	r2,	#16
dump__print_hex_loop:
	inc	r0
	mov	a,	r2
	anl	a,	#3	; mod 4
	jnz	dump__print_hex_no_gap
	mov	a,	#' ' ; gap after every 4 bytes
	acall	print_char
dump__print_hex_no_gap:
	mov	a,	r2
	setb	c
	subb	a,	r1
	jnc	dump__print_hex_no_padding
	mov	a,	#' ' ; padding for shorter line
	acall	print_char
	acall	print_char
	sjmp	dump__print_hex_next
dump__print_hex_no_padding:
	mov	a,	@r0
	acall	print_hex_8
dump__print_hex_next:
	mov	a,	#' ' ; space between bytes
	acall	print_char
	djnz	r2,	dump__print_hex_loop

	; wind back again
	mov	a,	r0
	add	a,	#-16
	mov	r0,	a

	; print ascii
	mov	a,	#' '
	acall	print_char
	mov	a,	#'|'
	acall	print_char
	mov	a,	#16
	clr	c
	subb	a,	r1
	mov	r1,	a ; number of bytes to print
	mov	r2,	a
dump__print_ascii_loop:
	inc	r0
	mov	a,	@r0
	cjne	a,	#0x20,	dump__print_ascii_check1
dump__print_ascii_check1:
	jc	dump__print_ascii_replace
	cjne	a,	#0x7f,	dump__print_ascii_check2
dump__print_ascii_check2:
	jc	dump__print_ascii_continue
dump__print_ascii_replace:
	mov	a,	#'.'
dump__print_ascii_continue:
	acall	print_char
	djnz	r2,	dump__print_ascii_loop
	mov	a,	#'|'
	acall	print_char
	mov	a,	#13
	acall	print_char
	mov	a,	#10
	acall	print_char

	; wind back again
	mov	a,	r0
	add	a,	#-16
	mov	r0,	a

	; next address
	mov	a,	r4
	add	a,	r1
	mov	r4,	a
	mov	a,	r5
	addc	a,	#0
	mov	r5,	a
	mov	a,	r6
	addc	a,	#0
	mov	r6,	a
	mov	a,	r7
	addc	a,	#0
	mov	r7,	a

	mov	a,	b
	jz	dump__end
	ajmp	dump__line_loop

dump__end:
	; deallocate array from stack
	mov	a,	sp
	add	a,	#-16
	mov	sp,	a

	acall	xch_r0123_r4567

	pop	acc
	mov	r7,	a
	pop	acc
	mov	r6,	a
	pop	acc
	mov	r5,	a
	pop	acc
	mov	r4,	a
	pop	acc
	pop	b
	ret
