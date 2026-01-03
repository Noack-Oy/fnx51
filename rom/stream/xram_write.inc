
; *** stream/xram_write.inc ***

; >> void stream_xram_write(char a) <<

; Write byte to on-chip expanded RAM at stream output pointer.
; 16-bit value of the stream pointer is used and incremented.

stream_xram_write:
	push	auxr
	push	dpl
	push	dph

	anl	auxr,#0xe1	; clear extram, xrs0-2
	orl	auxr,#0x10	; set xrs2 (size 1792 bytes)

	mov	dpl,stream_out
	mov	dph,stream_out+1

	movx	@dptr,a
	inc	dptr
	mov	stream_out,dpl
	mov	stream_out+1,dph

	pop	dph
	pop	dpl
	pop	auxr
	ret
