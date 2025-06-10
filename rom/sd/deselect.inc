
; *** sd/deselect.inc ***

; >> void sd_deselect() <<

; Deactivate the SD card
sd_deselect:
	push	acc
	mov	a,#0xff
	acall	spi_transfer
	setb	sd_pin_cs
	mov	a,#0xff
	acall	spi_transfer
	pop	acc
	ret
