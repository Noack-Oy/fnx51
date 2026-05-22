
; *** memory/allocate.inc ***

; >> r0r1 *memory_allocate(r0r1 size) <<

; Allocate memory, return lowest address

memory_allocate:
	push	ie
	clr	ea

	push	acc
	push	dpl
	push	dph
	inc	auxr1
	push	dpl
	push	dph

	; r0,r1 = size (input, will be overwritten with return value)
	; r2-r7 are clobbered, save them
	push	2
	push	3
	push	4
	push	5
	push	6
	push	7

	; size lives in r0r1 throughout; replaced with result before exit

	mov	a,r0
	orl	a,r1
	jnz	memory_allocate__0
	acall	panic	; invalid size (0)

memory_allocate__0:
	; rounding to next multiple of 4
	mov	a,r0
	add	a,#3
	anl	a,#0xfc
	mov	r0,a
	mov	a,r1
	addc	a,#0
	mov	r1,a

	; set prev (r2,r3) and curr (r4r5) pointers
	mov	r2,memory_list
	mov	r3,memory_list+1
	mov	dpl,r2
	mov	dph,r3
	movx	a,@dptr
	mov	r4,a
	inc	dptr
	movx	a,@dptr
	mov	r5,a

memory_allocate__loop:
	mov	a,r4
	orl	a,r5
	jnz	memory_allocate__0a
	acall	panic	; no large enough free block found
memory_allocate__0a:
	; compare curr->size with required size
	mov	dpl,r4
	mov	dph,r5
	inc	dptr
	inc	dptr	; dptr is now &(curr->size)
	movx	a,@dptr
	mov	r6,a	; store low byte for later (cannot easily dec dptr)
	inc	dptr	; compare high byte first
	clr	c
	movx	a,@dptr
	subb	a,r1
	jnz	memory_allocate__1
	mov	a,r6
	subb	a,r0
	jnz	memory_allocate__1

	; block size is exact match
	mov	dpl,r2	; prev
	mov	dph,r3
	inc	auxr1	;*switch data pointers
	mov	dpl,r4	; curr
	mov	dph,r5
	; prev->next = curr->next
	movx	a,@dptr
	inc	dptr
	inc	auxr1	;*
	movx	@dptr,a
	inc	dptr
	inc	auxr1	;*
	movx	a,@dptr
	inc	auxr1	;*
	movx	@dptr,a
	sjmp	memory_allocate__success

memory_allocate__1:
	jnc	memory_allocate__2 ; jump if curr is larger than required
	; too small, move on to next entry in list
	mov	a,r4	; -.
	mov	r2,a	;  |- prev = curr
	mov	a,r5	;  |
	mov	r3,a	; -'
	mov	dpl,r4	; -.
	mov	dph,r5	;  |
	movx	a,@dptr ;  |
	mov	r4,a	;  |- curr = curr->next
	inc	dptr	;  |
	movx	a,@dptr ;  |
	mov	r5,a	; -'
	sjmp	memory_allocate__loop

memory_allocate__2:
	; split big block
	mov	dpl,r2	; prev
	mov	dph,r3	;
	mov	a,r4	; -.
	add	a,r0	;  |
	mov	r2,a	;  |
	movx	@dptr,a ;  |
	inc	dptr	;  |- prev->next = curr+size
	mov	a,r5	;  |               '---.---'
	addc	a,r1	;  |  pointer to leftover block
	mov	r3,a	;  |  also gets stored in r2r3
	movx	@dptr,a ; -' (prev is not needed anymore)

	mov	dpl,r2  ; leftover
	mov	dph,r3
	inc	auxr1	;*switch data pointers
	mov	dpl,r4	; curr
	mov	dph,r5
	movx	a,@dptr	; -.
	inc	dptr	;  |
	inc	auxr1	;* |
	movx	@dptr,a ;  |
	inc	dptr	;  |
	inc	auxr1	;* |- leftover->next = curr->next
	movx	a,@dptr ;  |
	inc	dptr	;  |
	inc	auxr1	;* |
	movx	@dptr,a ;  |
	inc	dptr	; -'
	; data pointers are now &(leftover->size) and &(curr->size)
	inc	auxr1	;*-.
	movx	a,@dptr ;  |
	inc	dptr	;  |
	clr	c	;  |
	subb	a,r0	;  |
	inc	auxr1	;* |
	movx	@dptr,a ;  |- leftover->size = curr->size - size
	inc	dptr	;  |
	inc	auxr1	;* |
	movx	a,@dptr ;  |
	subb	a,r1	;  |
	inc	auxr1	;* |
	movx	@dptr,a ; -'

memory_allocate__success:
	mov	dpl,memory_list
	mov	dph,memory_list+1
	inc	dptr	; -.
	inc	dptr	;  |
	movx	a,@dptr ;  |
	clr	c	;  |
	subb	a,r0	;  |- memory_list->free -= size
	movx	@dptr,a ;  |
	inc	dptr	;  |
	movx	a,@dptr ;  |
	subb	a,r1	;  |
	movx	@dptr,a ; -'

	; replace size in r0r1 with the result pointer (curr in r4r5)
	mov	a,r4
	mov	r0,a
	mov	a,r5
	mov	r1,a

	pop	7
	pop	6
	pop	5
	pop	4
	pop	3
	pop	2
	pop	dph
	pop	dpl
	inc	auxr1
	pop	dph
	pop	dpl
	pop	acc

	pop	ie
	ret
