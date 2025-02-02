
; *** memory/release.inc ***

; >> void release(r0r1 *addr, r2r3 size) <<

; Allocate memory, return lowest address

memory_release:
	push	ie
	clr	ea
	
	push	acc
	push	dpl
	push	dph

	acall	regbank_next

	mov	a,r0
	add	a,#3
	mov	r0,a
	; r6r7 <- size
	mov	a,@r0
	mov	r7,a
	dec	r0
	mov	a,@r0
	mov	r6,a
	dec	r0
	; r0r1 <- pointer to block
	mov	a,@r0
	mov	r1,a
	dec	r0
	mov	a,@r0
	mov	r0,a

	; input parameter check
	mov	a,r6
	orl	a,r7
	jnz	memory_release__1
	acall	panic	; invalid size (0)
memory_release__1:
	mov	a,r1
	cjne	a,memory_start+1,memory_release__2
	mov	a,r0
	cjne	a,memory_start,memory_release__2
memory_release__2:
	jnc	memory_release__3
	acall	panic	; invalid address (< memory_start)
memory_release__3:
	mov	a,r1
	cjne	a,memory_end+1,memory_release__4
	mov	a,r0
	cjne	a,memory_end,memory_release__4
memory_release__4:
	jc	memory_release__5
	acall	panic	; invalid address (>= memory_end)
memory_release__5:

	; rounding
	mov	a,r6
	add	a,#3
	anl	a,#0xfc
	mov	r6,a
	mov	a,r7
	addc	a,#0
	mov	r7,a

	; find position in list
	; r2r3: prev pointer
	; r4r5: next pointer
	mov	r2,memory_list
	mov	r3,memory_list+1
	mov	dpl,r2
	mov	dph,r3
	movx	a,@dptr
	mov	r4,a
	inc	dptr
	movx	a,@dptr
	mov	r5,a
memory_release__loop:
	mov	a,r4
	orl	a,r5
	jz	memory_release__7	; next == null
	clr	c
	mov	a,r5
	subb	a,r1
	jnz	memory_release__6
	mov	a,r4
	subb	a,r0
	jnz	memory_release__6
	acall	panic	; double free	(next == block)
memory_release__6:
	jnc	memory_release__7	; next > block
	mov	a,r4	; -.
	mov	r2,a	;  |- prev = next
	mov	a,r5	;  |
	mov	r3,a	; -'
	mov	dpl,r4	; -.
	mov	dph,r5	;  |
	movx	a,@dptr ;  |
	mov	r4,a	;  |- next = next->next
	inc	dptr	;  |
	movx	a,@dptr ;  |
	mov	r5,a	; -'

	sjmp	memory_release__loop

memory_release__7:
	; if (prev == memory_list) ...
	mov	a,r3
	cjne	a,memory_list+1,memory_release__8
	mov	a,r2
	cjne	a,memory_list,memory_release__8
	sjmp	memory_release__link_new ; ... link as new first block ...
	; ... else:
memory_release__8:
	; compute top of previous block
	mov	dpl,r2
	mov	dph,r3
	inc	dptr
	inc	dptr	; prev->size
	; top = prev + prev->size
	movx	a,@dptr
	add	a,r2
	push	acc	;-.
	inc	dptr	; |
	movx	a,@dptr ; |
	addc	a,r3	; |
	mov	dph,a	; |
	pop	dpl	;<' dptr = top

	; if (block == top) ...
	mov	a,r1
	cjne	a,dph,memory_release__9
	mov	a,r0
	cjne	a,dpl,memory_release__9
	; ... coalsece with previous block ...
	mov	dpl,r2
	mov	dph,r3
	inc	dptr
	inc	dptr	; prev->size
	movx	a,@dptr ; -.
	add	a,r6	;  |- += size
	movx	@dptr,a ;  |
	inc	dptr	;  |
	movx	a,@dptr ;  |
	addc	a,r7	;  |
	movx	@dptr,a ; -'
	mov	a,r2	; -.
	mov	r0,a	;  |- block = prev
	mov	a,r3	;  |
	mov	r1,a	; -'
	sjmp	memory_release__10
	; ... else:
memory_release__9:
	jnc	memory_release__link_new ; ... link as new block in the middle
	acall	panic	; error: block < top (overlap)

memory_release__link_new:
	; link into list as new node
	mov	dpl,r0	; -.
	mov	dph,r1	;  |
	mov	a,r4	;  |
	movx	@dptr,a ;  |- block->next = next
	inc	dptr	;  |
	mov	a,r5	;  |
	movx	@dptr,a ; -'
	inc	dptr	; -.
	mov	a,r6	;  |
	movx	@dptr,a ;  |- block->size = size
	inc	dptr	;  |
	mov	a,r7	;  |
	movx	@dptr,a ; -'
	mov	dpl,r2	; -.
	mov	dph,r3	;  |
	mov	a,r0	;  |
	movx	@dptr,a ;  |- prev->next = block
	inc	dptr	;  |
	mov	a,r1	;  |
	movx	@dptr,a ; -'
	; last use of prev in r2r3

memory_release__10:
	; check if block is touching next block
	mov	dpl,r0
	mov	dph,r1
	inc	dptr
	inc	dptr	; block->size
	movx	a,@dptr
	add	a,r0
	mov	r2,a	; r2 used as temp storage here
	inc	dptr
	movx	a,@dptr
	addc	a,r1
	mov	dph,a
	mov	dpl,r2 ; dptr: block + block->size

	mov	a,r5
	cjne	a,dph,memory_release__11
	mov	a,r4
	cjne	a,dpl,memory_release__11

	; coalesce with next block
	mov	dpl,r0	; -.
	mov	dph,r1	;  |
	inc	auxr1	; *|
	mov	dpl,r4	;  |
	mov	dph,r5	;  |
	movx	a,@dptr ;  |
	inc	dptr	;  |
	inc	auxr1	;* |- block->next = next->next
	movx	@dptr,a ;  |
	inc	dptr	;  |
	inc	auxr1	; *|
	movx	a,@dptr ;  |
	inc	dptr	;  |
	inc	auxr1	;* |
	movx	@dptr,a ; -'
	inc	dptr	; -.
	inc	auxr1	; *|
	movx	a,@dptr ;  |
	inc	dptr	;  |
	mov	r2,a	;  |
	inc	auxr1	;* |
	movx	a,@dptr ;  |
	add	a,r2	;  |
	movx	@dptr,a ;  |- block->size += next->size
	inc	dptr	;  |
	inc	auxr1	; *|
	movx	a,@dptr ;  |
	mov	r3,a	;  |
	inc	auxr1	;* |
	movx	a,@dptr ;  |
	addc	a,r3	;  |
	movx	@dptr,a ; -'
	sjmp	memory_release__end

memory_release__11:
	jnc	memory_release__end
	acall	panic	; error: next < block+block->size

memory_release__end:
	mov	dpl,memory_list
	mov	dph,memory_list+1
	inc	dptr
	inc	dptr
	movx	a,@dptr
	add	a,r6
	movx	@dptr,a
	inc	dptr
	movx	a,@dptr
	addc	a,r7
	movx	@dptr,a

	acall	regbank_prev
	pop	dph
	pop	dpl
	pop	acc
	pop	ie
	ret
