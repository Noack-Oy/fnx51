
; *** stream/imem_read.inc ***

; >> char stream_imem_read() <<

; Read byte from internal RAM at stream input pointer.
; Only the lower byte of the stream pointer is used and incremented.

stream_imem_read:
	xch	a,	r0
	xch	a,	stream_in
	xch	a,	r0
	mov	a,	@r0
	inc	r0
	xch	a,	r0
	xch	a,	stream_in
	xch	a,	r0
	ret
