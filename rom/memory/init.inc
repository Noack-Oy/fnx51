
; *** memory/init.inc ***

; >> void memory_init(() <<

; Enable xram, initialize memory allocator data structures

memory_init:

	push	acc
	push	dpl
	push	dph

	; use xram
	anl	auxr,#0xe1	; clear extram, xrs0-2
	orl	auxr,#0x10	; set xrs2 (size 1792 bytes)

	; clear xram with dummy value
	mov	dptr,#0
memory_init__1:
	mov	a,#0x23
	movx	@dptr,a
	inc	dptr
	mov	a,#0x07 ; xram goes up to 0x06ff
	cjne	a,dph,memory_init__1

	; initialize memory list
	mov	dptr,#0
	mov	memory_list,dpl
	mov	memory_list+1,dph
	; list head: pointer to first block
	mov	a,#4
	mov	memory_start,a
	movx	@dptr,a
	inc	dptr
	clr	a
	mov	memory_start+1,a
	movx	@dptr,a
	inc	dptr
	; max address of allocatable memory: 0x06ff
	mov	memory_end,#0xff
	mov	memory_end+1,0x06
	; amount of free memory: 1792-4 = 1788 = 0x06fc
	mov	a,#0xfc
	movx	@dptr,a
	inc	dptr
	mov	a,#0x06
	movx	@dptr,a
	inc	dptr
	; first entry: pointer to next (null initially)
	clr	a
	movx	@dptr,a
	inc	dptr
	movx	@dptr,a
	inc	dptr
	; size of block (same as free memory initially)
	mov	a,#0xfc
	movx	@dptr,a
	inc	dptr
	mov	a,#0x06
	movx	@dptr,a
	inc	dptr

	pop	dph
	pop	dpl
	pop	acc

	ret
