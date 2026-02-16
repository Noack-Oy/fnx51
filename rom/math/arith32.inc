
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
