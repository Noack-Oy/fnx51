
; *** read/char.inc ***

; >> int32_t read_hex_32() <<

; Read 32 bit hex integer from input into r0-r3.
; Stops at the first non-hex character or integer overflow.

read_hex_32:
	push	acc
	mov	a,	r4
	push	acc
	mov	a,	r5
	push	acc

	clr	a
	mov	r0,	a
	mov	r1,	a
	mov	r2,	a
	mov	r3,	a

read_hex_32__loop:
	acall	read_char

	clr	c
	subb	a,	#0x30	; ascii '0'
	jc	read_hex_32__end
	cjne	a,	#10,	read_hex_32__1
read_hex_32__1:
	jc	read_hex_32__3
	subb	a,	#17	; chars between '0' and 'A'
	jc	read_hex_32__end
	add	a,	#10
	anl	a,	#0xdf	; allow lowercase
	cjne	a,	#0x10,	read_hex_32__2
read_hex_32__2:
	jnc	read_hex_32__end

read_hex_32__3:
	mov	r4,	a
	mov	a,	r0
	swap	a
	mov	r5,	a
	anl	a,	#0xf0
	orl	a,	r4
	mov	r0,	a
	mov	a,	r5
	anl	a,	#0x0f

	mov	r4,	a
	mov	a,	r1
	swap	a
	mov	r5,	a
	anl	a,	#0xf0
	orl	a,	r4
	mov	r1,	a
	mov	a,	r5
	anl	a,	#0x0f

	mov	r4,	a
	mov	a,	r2
	swap	a
	mov	r5,	a
	anl	a,	#0xf0
	orl	a,	r4
	mov	r2,	a
	mov	a,	r5
	anl	a,	#0x0f

	mov	r4,	a
	mov	a,	r3
	swap	a
	mov	r5,	a
	anl	a,	#0xf0
	orl	a,	r4
	mov	r3,	a
	mov	a,	r5
	anl	a,	#0x0f

	jnz	read_hex_32__end	; overflow
	sjmp	read_hex_32__loop


read_hex_32__end:
	pop	acc
	mov	r5,	a
	pop	acc
	mov	r4,	a
	pop	acc
	ret
