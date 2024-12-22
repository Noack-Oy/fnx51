
; *** stream/xram_read.inc ***

; >> char stream_xram_read() <<

; Read byte from on-chip expanded RAM at stream input pointer.
; 16 bit value of the stream pointer is used and incremented.

stream_xram_read:
	push	auxr
	push	dpl
	push	dph

	mov	a,	auxr
	anl	a,	0xe1	; clear extram, xrs0-2
	orl	a,	0x10	; set xrs2 (size 1792 bytes)
	mov	auxr,	a

	mov	dpl,	stream_in
	mov	dph,	stream_in+1

	movx	a,	@dptr
	inc	dptr
	mov	stream_in,	dpl
	mov	stream_in+1,	dph

	pop	dph
	pop	dpl
	pop	auxr
	ret
