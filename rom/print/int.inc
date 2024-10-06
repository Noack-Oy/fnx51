
; *** print/int.inc ***

; >> void print_int_s8(char a) <<

; Print signed integer from accumulator.

print_int_s8:
	push	ACC
	push	B
	jnb	ACC.7,	print_int_u8__1
	mov	b,	a
	mov	a,	#'-'
	acall	print_char
	mov	a,	b
	cpl	a
	inc	a
	sjmp	print_int_u8__1

; >> void print_int_u8(char a) <<

; Print unsingned integer from accumulator.

print_int_u8:
	push	ACC
	push	B
print_int_u8__1:
	mov	b,	r0
	push	B

	mov	r0,	#0
	mov	b,	#100
	div	ab
	jz	print_int_u8__tens
	add	a,	#'0'
	acall	print_char
	inc	r0	; stop skipping leading zeroes
print_int_u8__tens:
	mov	a,	b
	mov	b,	#10
	div	ab
	add	a,	r0
	jz	print_int_u8__ones
	subb	a,	r0
	add	a,	#'0'
	acall	print_char
print_int_u8__ones:
	mov	a,	b
	add	a,	#'0'
	acall	print_char

	pop	ACC
	mov	r0,	a
	pop	B
	pop	ACC
	ret

; >> void print_int_s16(int r0r1) <<

; Print signed integer from r0-r1.

print_int_s16:
	push	ACC
	mov	a,	r0
	push	ACC
	mov	a,	r1
	push	ACC
	jnb	ACC.7,	print_int_u16__1
	mov	a,	#'-'
	acall	print_char
	mov	a,	r0
	cpl	a
	add	a,	#1
	mov	r0,	a
	mov	a,	r1
	cpl	a
	addc	a,	#0
	mov	r1,	a
	sjmp	print_int_u16__1


; >> void print_int_u16(int r0r1) <<

; Print unsigned integer from r0-r1.

print_int_u16:
	push	ACC
	mov	a,	r0
	push	ACC
	mov	a,	r1
	push	ACC
print_int_u16__1:
	mov	a,	r3
	push	ACC
	push	B

	mov	r2,	#0 ; flag for leading zeroes

	; divide by 10000 (0x2710)
	; collect quotient in b
	mov	b,	#-1
	clr	c
print_int_u16__10k_loop:
	mov	a,	r0
	subb	a,	#0x10
	mov	r0,	a
	mov	a,	r1
	subb	a,	#0x27
	mov	r1,	a
	inc	b
	jnc	print_int_u16__10k_loop
	; undo last subtraction to restore remainder
	mov	a,	r0
	add	a,	#0x10
	mov	r0,	a
	mov	a,	r1
	addc	a,	#0x27
	mov	r1,	a
	; print tenthousands digit
	mov	a,	b
	jz	print_int_u16__1k
	add	a,	#'0'
	acall	print_char
	inc	r2
print_int_u16__1k:
	; divide by 1000 (0x03e8)
	; collect quotient in b
	mov	b,	#-1
	clr	c
print_int_u16__1k_loop:
	mov	a,	r0
	subb	a,	#0xe8
	mov	r0,	a
	mov	a,	r1
	subb	a,	#0x03
	mov	r1,	a
	inc	b
	jnc	print_int_u16__1k_loop
	; undo last subtraction to restore remainder
	mov	a,	r0
	add	a,	#0xe8
	mov	r0,	a
	mov	a,	r1
	addc	a,	#0x03
	mov	r1,	a
	; print thousands digit
	mov	a,	b
	add	a,	r2
	jz	print_int_u16__100
	subb	a,	r2
	add	a,	#'0'
	acall	print_char
	inc	r2
print_int_u16__100:
	; divide by 100 (0x0064)
	; collect quotient in b
	mov	b,	#-1
	clr	c
print_int_u16__100_loop:
	mov	a,	r0
	subb	a,	#0x64
	mov	r0,	a
	mov	a,	r1
	subb	a,	#0x00
	mov	r1,	a
	inc	b
	jnc	print_int_u16__100_loop
	; undo last subtraction to restore remainder
	mov	a,	r0
	add	a,	#0x64
	mov	r0,	a
	mov	a,	r1
	addc	a,	#0x00
	mov	r1,	a
	; print hundreds digit
	mov	a,	b
	add	a,	r2
	jz	print_int_u16__tens
	subb	a,	r2
	add	a,	#'0'
	acall	print_char
	inc	r2
print_int_u16__tens:
	; divde by 10 (using div instruction)
	mov	a,	r0
	mov	b,	#10
	div	ab
	; print tens digit
	add	a,	r2
	jz	print_int_u16__ones
	subb	a,	r2
	add	a,	#'0'
	acall	print_char
print_int_u16__ones:
	; print ones digit
	mov	a,	b
	add	a,	#'0'
	acall	print_char

	pop	B
	pop	ACC
	mov	r2,	a
	pop	ACC
	mov	r1,	a
	pop	ACC
	mov	r0,	a
	pop	ACC
	ret
