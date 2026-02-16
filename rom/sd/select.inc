
; *** sd/select.inc ***

; >> void sd_select() <<

; Activate the SD card
sd_select:
	push	acc
	mov	a,#0xff
	acall	spi_transfer
	clr	sd_pin_cs
	mov	a,#0xff
	acall	spi_transfer
	pop	acc
	ret
