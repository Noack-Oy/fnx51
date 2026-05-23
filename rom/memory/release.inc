
; *** memory/release.inc ***

; >> void memory_release(char *{dptr} addr, uint16{r0r1} size) <<

; Release a previously allocated block. `addr` must be the pointer
; returned by memory_allocate, `size` the same value passed to it.

memory_release:
	push	ie
	clr	ea

	push	acc
	push	dpl
	push	dph

	; void function: r0-r7 all preserved
	push	0
	push	1
	push	2
	push	3
	push	4
	push	5
	push	6
	push	7

	; copy address from dptr into r2r3 for the body's use;
	; size stays in r0r1 (where the caller put it)
	mov	r2,dpl
	mov	r3,dph

	; r0r1: size, r2r3: block addr;
	; r6r7 takes over the prev-pointer role during the list walk.

	; input parameter check
	mov	a,r0
	orl	a,r1
	jnz	memory_release__1
	lcall	panic	; invalid size (0)
memory_release__1:
	mov	a,r3
	cjne	a,memory_start+1,memory_release__2
	mov	a,r2
	cjne	a,memory_start,memory_release__2
memory_release__2:
	jnc	memory_release__3
	lcall	panic	; invalid address (< memory_start)
memory_release__3:
	mov	a,r3
	cjne	a,memory_end+1,memory_release__4
	mov	a,r2
	cjne	a,memory_end,memory_release__4
memory_release__4:
	jc	memory_release__5
	lcall	panic	; invalid address (>= memory_end)
memory_release__5:

	; rounding
	mov	a,r0
	add	a,#3
	anl	a,#0xfc
	mov	r0,a
	mov	a,r1
	addc	a,#0
	mov	r1,a

	; find position in list
	; r6r7: prev pointer
	; r4r5: next pointer
	mov	r6,memory_list
	mov	r7,memory_list+1
	mov	dpl,r6
	mov	dph,r7
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
	subb	a,r3
	jnz	memory_release__6
	mov	a,r4
	subb	a,r2
	jnz	memory_release__6
	lcall	panic	; double free	(next == block)
memory_release__6:
	jnc	memory_release__7	; next > block
	mov	a,r4	; -.
	mov	r6,a	;  |- prev = next
	mov	a,r5	;  |
	mov	r7,a	; -'
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
	mov	a,r7
	cjne	a,memory_list+1,memory_release__8
	mov	a,r6
	cjne	a,memory_list,memory_release__8
	sjmp	memory_release__link_new ; ... link as new first block ...
	; ... else:
memory_release__8:
	; compute top of previous block
	mov	dpl,r6
	mov	dph,r7
	inc	dptr
	inc	dptr	; prev->size
	; top = prev + prev->size
	movx	a,@dptr
	add	a,r6
	push	acc	;-.
	inc	dptr	; |
	movx	a,@dptr ; |
	addc	a,r7	; |
	mov	dph,a	; |
	pop	dpl	;<' dptr = top

	; if (block == top) ...
	mov	a,r3
	cjne	a,dph,memory_release__9
	mov	a,r2
	cjne	a,dpl,memory_release__9
	; ... coalsece with previous block ...
	mov	dpl,r6
	mov	dph,r7
	inc	dptr
	inc	dptr	; prev->size
	movx	a,@dptr ; -.
	add	a,r0	;  |- += size
	movx	@dptr,a ;  |
	inc	dptr	;  |
	movx	a,@dptr ;  |
	addc	a,r1	;  |
	movx	@dptr,a ; -'
	mov	a,r6	; -.
	mov	r2,a	;  |- block = prev
	mov	a,r7	;  |
	mov	r3,a	; -'
	sjmp	memory_release__10
	; ... else:
memory_release__9:
	jnc	memory_release__link_new ; ... link as new block in the middle
	lcall	panic	; error: block < top (overlap)

memory_release__link_new:
	; link into list as new node
	mov	dpl,r2	; -.
	mov	dph,r3	;  |
	mov	a,r4	;  |
	movx	@dptr,a ;  |- block->next = next
	inc	dptr	;  |
	mov	a,r5	;  |
	movx	@dptr,a ; -'
	inc	dptr	; -.
	mov	a,r0	;  |
	movx	@dptr,a ;  |- block->size = size
	inc	dptr	;  |
	mov	a,r1	;  |
	movx	@dptr,a ; -'
	mov	dpl,r6	; -.
	mov	dph,r7	;  |
	mov	a,r2	;  |
	movx	@dptr,a ;  |- prev->next = block
	inc	dptr	;  |
	mov	a,r3	;  |
	movx	@dptr,a ; -'
	; last use of prev in r6r7

memory_release__10:
	; check if block is touching next block
	mov	dpl,r2
	mov	dph,r3
	inc	dptr
	inc	dptr	; block->size
	movx	a,@dptr
	add	a,r2
	mov	r6,a	; r6 used as temp storage here (prev is done with)
	inc	dptr
	movx	a,@dptr
	addc	a,r3
	mov	dph,a
	mov	dpl,r6 ; dptr: block + block->size

	mov	a,r5
	cjne	a,dph,memory_release__11
	mov	a,r4
	cjne	a,dpl,memory_release__11

	; coalesce with next block
	mov	dpl,r2	; -.
	mov	dph,r3	;  |
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
	mov	r6,a	;  |
	inc	auxr1	;* |
	movx	a,@dptr ;  |
	add	a,r6	;  |
	movx	@dptr,a ;  |- block->size += next->size
	inc	dptr	;  |
	inc	auxr1	; *|
	movx	a,@dptr ;  |
	mov	r7,a	;  |
	inc	auxr1	;* |
	movx	a,@dptr ;  |
	addc	a,r7	;  |
	movx	@dptr,a ; -'
	sjmp	memory_release__end

memory_release__11:
	jnc	memory_release__end
	lcall	panic	; error: next < block+block->size

memory_release__end:
	mov	dpl,memory_list
	mov	dph,memory_list+1
	inc	dptr
	inc	dptr
	movx	a,@dptr
	add	a,r0
	movx	@dptr,a
	inc	dptr
	movx	a,@dptr
	addc	a,r1
	movx	@dptr,a

	pop	7
	pop	6
	pop	5
	pop	4
	pop	3
	pop	2
	pop	1
	pop	0
	pop	dph
	pop	dpl
	pop	acc
	pop	ie
	ret
