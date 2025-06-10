
; *** sd/warmup.inc ***

; >> void sd_warmup() <<

; send 80 warmup pulses to sclk while cs is high
sd_warmup:
	push	acc
	push	b
	setb	sd_pin_cs ; p1.1
	mov	b,#10
sd_warmup_loop:
	mov	a,#0xff
	acall	spi_transfer
	djnz	b,sd_warmup_loop
	pop	b
	pop	acc
	ret
