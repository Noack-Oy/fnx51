
; *** block/init.inc ***

; >> void block_init() <<

; Initialize block device cache.

block_init:
	push	acc
	mov	a,r0
	push	acc
	mov	a,r1
	push	acc
	push	dpl
	push	dph

	mov	r0,#5	; 512+5
	mov	r1,#2
	acall	memory_allocate
	mov	block_cache,r0
	mov	block_cache+1,r1
	mov	dpl,r0
	mov	dph,r1
	mov	a,0xff
	movx	@dptr,a	; invalid current block address (0xffffffff)
	inc	dptr
	movx	@dptr,a
	inc	dptr
	movx	@dptr,a
	inc	dptr
	movx	@dptr,a
	inc	dptr
	clr	a
	movx	@dptr,a ; clear dirty flag initially

	pop	dph
	pop	dpl
	pop	acc
	mov	r1,a
	pop	acc
	mov	r0,a
	pop	acc
	ret
