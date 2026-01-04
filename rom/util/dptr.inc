
; *** util/dptr.inc ***

; >> void dptr_copy(uint8{r0} n, char *{dptr} &dst, char *{dptr'} &src) <<

; Use dual data pointers to copy n (given in r0) bytes to dst (initial
; dptr) from src (other dptr). In the end both pointers point to the
; first dst/src bytes after the copied blocks.

dptr_copy:
	push	acc
	mov	a,r0
	push	acc

dptr_copy__loop:
	inc	auxr1
	movx	a,@dptr
	inc	auxr1
	movc	@dptr,a
	djnz	r0,ddpr_copy__loop

	pop	acc
	mov	r0,a
	pop	acc
	ret


; >> uint32{r0-r3} dptr_read_32(uint32 *{dptr}&addr) <<

; Read 32 bit value into r0-r3 via movx from @dptr.
; Data pointer is returned in advanced position (first byte after).

dptr_read_32:
	push	acc

	movx	a,@dptr
	mov	r0,a
	inc	dptr

	movx	a,@dptr
	mov	r1,a
	inc	dptr

	movx	a,@dptr
	mov	r2,a
	inc	dptr

	movx	a,@dptr
	mov	r3,a
	inc	dptr

	pop	acc


; >> void dptr_write_32(uint32 *{dptr}&addr, uint32{r0-r3} value <<

; Write 32 bit value from r0-r3 via movx to @dptr.
; Data pointer is returned in advanced position (first byte after).

dptr_write_32:
	push	acc

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

	pop	acc
