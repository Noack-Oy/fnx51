
; *** stream/xram_read.inc ***

; >> char{a} stream_xram_read() <<

; Read byte from on-chip expanded RAM at stream input pointer.
; 16 bit value of the stream pointer is used and incremented.

stream_xram_read:
	push	auxr
	push	dpl
	push	dph

	anl	auxr,#0xe1	; clear extram, xrs0-2
	orl	auxr,#0x10	; set xrs2 (size 1792 bytes)

	mov	dpl,stream_in
	mov	dph,stream_in+1

	movx	a,@dptr
	inc	dptr
	mov	stream_in,dpl
	mov	stream_in+1,dph

	pop	dph
	pop	dpl
	pop	auxr
	ret
