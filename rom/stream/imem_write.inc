
; *** stream/imem_write.inc ***

; >> void stream_imem_write(char a) <<

; Write byte to internal RAM at stream output pointer.
; Only the lower byte of the stream pointer is used and incremented.

stream_imem_write:
	xch	a,	r0
	xch	a,	stream_out
	xch	a,	r0
	mov	@r0,	a
	inc	r0
	xch	a,	r0
	xch	a,	stream_out
	xch	a,	r0
	ret
