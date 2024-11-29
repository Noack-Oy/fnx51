
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
	; divide by 10 (using div instruction)
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

; >> void print_int_s32(long r0r1r2r3) <<

; Print signed integer from r0-r3.

print_int_s32:
	push	acc
	push	b
	push	dpl
	push	dph
	acall	regbank_next
	; r4-r7: value to print
	mov	a,	@r0
	mov	r4,	a
	inc	r0
	mov	a,	@r0
	mov	r5,	a
	inc	r0
	mov	a,	@r0
	mov	r6,	a
	inc	r0
	mov	a,	@r0
	mov	r7,	a
	jnb	ACC.7,	print_int_u32__1
	mov	a,	#'-'
	acall	print_char
	mov	a,	r4
	cpl	a
	add	a,	#1
	mov	r4,	a
	mov	a,	r5
	cpl	a
	addc	a,	#0
	mov	r5,	a
	mov	a,	r6
	cpl	a
	addc	a,	#0
	mov	r6,	a
	mov	a,	r7
	cpl	a
	addc	a,	#0
	mov	r7,	a
	sjmp	print_int_u32__1


; >> void print_int_u32(long r0r1r2r3) <<

; Print unsigned integer from r0-r3.

print_int_u32:
	push	acc
	push	b
	push	dpl
	push	dph
	acall	regbank_next
	; r4-r7: value to print
	mov	a,	@r0
	mov	r4,	a
	inc	r0
	mov	a,	@r0
	mov	r5,	a
	inc	r0
	mov	a,	@r0
	mov	r6,	a
	inc	r0
	mov	a,	@r0
	mov	r7,	a
print_int_u32__1:
	mov	b,	#0 ; flag for leading zeroes
	mov	dptr,	#print_int_u32__divisors
print_int_u32__loop:
	; get divisor
	clr	a
	movc	a,	@a+dptr
	mov	r0,	a
	inc	dptr
	clr	a
	movc	a,	@a+dptr
	mov	r1,	a
	inc	dptr
	clr	a
	movc	a,	@a+dptr
	mov	r2,	a
	inc	dptr
	clr	a
	movc	a,	@a+dptr
	mov	r3,	a
	inc	dptr
	; divide r4-r7 by r0-r3
	acall	print_int_u32__divide
	add	a,	b
	jz	print_int_u32__skip ; leading zero
	subb	a,	b
	add	a,	#'0'
	inc	b
	acall	print_char
print_int_u32__skip:
	cjne	r0,	#0x0a,	print_int_u32__loop
	mov	a,	r4
	add	a,	#'0'
	acall	print_char

	acall	regbank_prev
	pop	dph
	pop	dpl
	pop	b
	pop	acc
	ret
; Divisor table:
print_int_u32__divisors:
	; 1,000,000,000 = 3b9aca00
	.db	0x00, 0xca, 0x9a, 0x3b
	;   100,000,000 = 05f5e100
	.db	0x00, 0xe1, 0xf5, 0x05
	;    10,000,000 = 00989680
	.db	0x80, 0x96, 0x98, 0x00
	;     1,000,000 = 000f4240
	.db	0x40, 0x42, 0x0f, 0x00
	;       100,000 = 000186a0
	.db	0xa0, 0x86, 0x01, 0x00
	;        10,000 = 00002710
	.db	0x10, 0x27, 0x00, 0x00
	;         1,000 = 000003e8
	.db	0xe8, 0x03, 0x00, 0x00
	;           100 = 00000064
	.db	0x64, 0x00, 0x00, 0x00
	;            10 = 0000000a
	.db	0x0a, 0x00, 0x00, 0x00
; Divide r4-r7 by r0-r1 using repeated subtraction.
; Quotient returned in a, remainder in r4-r7.
; Since the quotient is at most 9,
; this is faster than 'proper' division.
print_int_u32__divide:
	push	b
	clr	c
	mov	b,	#-1
print_int_u32__divide_loop:
	mov	a,	r4
	subb	a,	r0
	mov	r4,	a
	mov	a,	r5
	subb	a,	r1
	mov	r5,	a
	mov	a,	r6
	subb	a,	r2
	mov	r6,	a
	mov	a,	r7
	subb	a,	r3
	mov	r7,	a
	inc	b
	jnc	print_int_u32__divide_loop
	; restore remainder
	mov	a,	r4
	add	a,	r0
	mov	r4,	a
	mov	a,	r5
	addc	a,	r1
	mov	r5,	a
	mov	a,	r6
	addc	a,	r2
	mov	r6,	a
	mov	a,	r7
	addc	a,	r3
	mov	r7,	a
	mov	a,	b
	pop	b
	ret
