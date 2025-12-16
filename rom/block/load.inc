
; *** block/load.inc ***

; >> char *{dptr}block_load(uint32{r0-r3} &block_address) <<

; Load a block into memory if necessary, return pointer to data.
; If the requested block is already cached, no read is done.
; Loading a block makes all previously returned data pointers invalid.
; Effectively, only the most recently loaded block is guaranteed to be
; present in memory, and is called the "current" block.
; If the current block is dirty and needs to be evicted from memory,
; it is flushed automatically before loading another one.

block_load:
	push	acc
	mov	r4,a
	push	acc
	mov	r5,a
	push	acc

	mov	dpl,block_cache
	mov	dph,block_cache+1

	; compare requested address with current
	movx	a,@dptr
	inc	dptr
	xrl	a,r0
	jnz	block_load__do_read_1

	movx	a,@dptr
	inc	dptr
	xrl	a,r1
	jnz	block_load__do_read_2

	movx	a,@dptr
	inc	dptr
	xrl	a,r2
	jnz	block_load__do_read_3

	movx	a,@dptr
	inc	dptr
	xrl	a,r3
	jnz	block_load__do_read_4

	; requested block is already cached
	inc	dptr ; skip dirty flag, points to data now
	sjmp	block_load__end

block_load__do_read_1:
	inc	dptr
block_load__do_read_2:
	inc	dptr
block_load__do_read_3:
	inc	dptr
block_load__do_read_4:

	; check dirty flag:
	movx	a,@dptr
	inc	dptr
	jz	block_load__do_read_5
	acall	block_flush

block_load__do_read_5:
	acall	block_load__read_icall
	; TODO: check CRC (r4-r5)

	; save current block address
	mov	dpl,block_cache
	mov	dph,block_cache+1
	mov	a,r0
	movx	@dptr,a
	inc	dptr
	mov	a,r1
	movx	@dptr,a
	inc	dptr
	mov	a,r2
	movx	@dptr,a
	inc	dptr
	mov	a,r3
	movx	@dptr,a
	inc	dptr
	; clear dirty flag
	clr	a
	movx	@dptr,a
	inc	dptr
	; dptr points to loaded block data now

block_load__end:
	pop	acc
	mov	r5,a
	pop	acc
	mov	r4,a
	pop	acc
	ret

block_load__read_icall:
	push	block_read
	push	block_read+1
	ret	; indirect jump
