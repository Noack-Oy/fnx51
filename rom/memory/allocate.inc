
; *** memory/allocate.inc ***

; >> char *getmem(uint16 size) <<

; Allocate memory, return lowest address

memory_allocate:
	push	ie
	clr	ea

	push	acc
	mov	a,	r2
	push	acc
	mov	a,	r3
	push	acc
	mov	a,	r4
	push	acc
	mov	a,	r5
	push	acc

	; r0r1: size (param)
	; r2r3: prev pointer
	mov	r2,	memlist
	mov	r3,	memlist+1
	; r4r5: curr pointer
	mov	dpl,	r2
	mov	dph,	r3
	mov	r4,	@dptr
	inc	dptr
	mov	r5,	@dptr

	pop	acc
	mov	r5,	a
	pop	acc
	mov	r4,	a
	pop	acc
	mov	r3,	a
	pop	acc
	mov	r2,	a
	pop	acc

	pop	ie
	ret
