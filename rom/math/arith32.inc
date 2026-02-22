
; *** math/arith32.inc ***

; >> uint32{r0-r3} add_32(uint32{r0-r3} x, uint32{r4-r7} y) <<

; 32 bit addition, x := x + y

add_32:
	push	acc

	mov	a,r0
	add	a,r4
	mov	r0,a

	mov	a,r1
	addc	a,r5
	mov	r1,a

	mov	a,r2
	addc	a,r6
	mov	r2,a

	mov	a,r3
	addc	a,r7
	mov	r3,a

	pop	acc
	ret


; >> uint32{r0-r3} sub_32(uint32{r0-r3} x, uint32{r4-r7} y) <<

; 32 bit subtraction, x := x - y

sub_32:
	push	acc

	clr	c

	mov	a,r0
	subb	a,r4
	mov	r0,a

	mov	a,r1
	subb	a,r5
	mov	r1,a

	mov	a,r2
	subb	a,r6
	mov	r2,a

	mov	a,r3
	subb	a,r7
	mov	r3,a

	pop	acc
	ret


; >> uint32{r0-r3} shl_32(uint32{r0-r3} value, uint8{a} count) <<

; Shift 32-bit value left by count bits (count in a).
; Equivalent to: value <<= count

shl_32:
	push	acc
	push	b

	mov	b,a			; shift count to b for loop counter
shl_32__loop:
	mov	a,b
	jz	shl_32__done		; when b==0, done
	dec	b			; decrement shift counter
	
	; Shift r0-r3 left by 1 position
	clr	c
	mov	a,r0
	rlc	a
	mov	r0,a
	mov	a,r1
	rlc	a
	mov	r1,a
	mov	a,r2
	rlc	a
	mov	r2,a
	mov	a,r3
	rlc	a
	mov	r3,a
	sjmp	shl_32__loop

shl_32__done:
	pop	b
	pop	acc
	ret
