
; *** print/hex.inc ***

; >> void print_hex_8(char a) <<

; Print hexadecimal number from accumulator.

print_hex_8:
	swap	a	; high nibble first
	acall	print_hex_8__nibble
	swap	a
print_hex_8__nibble:
	push	ACC
	anl	a,	#0x0f
	add	a,	#0x90
	da	a
	addc	a,	#0x40
	da	a
	orl	a,	#0x20	; lower case
	acall	print_char
	pop	ACC
	ret

; >> void print_hex_16(int r0r1) <<

; Print hexadecimal number from r0-r1.

print_hex_16:
	push	ACC
	mov	a,	r1	; MSB first
	acall	print_hex_8
	mov	a,	r0	; LSB second
	acall	print_hex_8
	pop	ACC
	ret

; >> void print_hex_32(long r0r1r2r3) <<

; Print hexadecimal number from r0-r3.

print_hex_32:
	push	ACC
	mov	a,	r3	; MSB first
	acall	print_hex_8
	mov	a,	r2
	acall	print_hex_8
	mov	a,	r1
	acall	print_hex_8
	mov	a,	r0	; LSB last
	acall	print_hex_8
	pop	ACC
	ret
