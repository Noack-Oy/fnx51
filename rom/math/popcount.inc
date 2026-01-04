
; *** math/popcount.inc ***

; >> uint8{a} popcount_8(uint8{a} value) <<

; Count the number of "1" bits in a, return result in a

popcount_8:
	push	b
	mov	b,a
	anl	a,#0x0f
	acall	popcount_8__nibble
	xch	a,b
	swap	a
	anl	a,#0x0f
	acall	popcount_8__nibble
	add	a,b
	pop	b
	ret

popcount_8__nibble:
	inc	a
	movc	a,@a+pc
	ret
	.db	0 ; 0000 (0)
	.db	1 ; 0001 (1)
	.db	1 ; 0010 (2)
	.db	2 ; 0011 (3)
	.db	1 ; 0100 (4)
	.db	2 ; 0101 (5)
	.db	2 ; 0110 (6)
	.db	3 ; 0111 (7)
	.db	1 ; 1000 (8)
	.db	2 ; 1001 (9)
	.db	2 ; 1010 (a)
	.db	3 ; 1011 (b)
	.db	2 ; 1100 (c)
	.db	3 ; 1101 (d)
	.db	3 ; 1110 (e)
	.db	4 ; 1111 (f)
