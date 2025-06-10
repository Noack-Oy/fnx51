
; *** sd/command.inc ***

; >> void sd_command(uint8{a} cmd, uint32{r0-r3} arg, uint8{r4} crc) <<

; Issue a command to the SD card
sd_command:
	push	acc
	; transmit command
	orl	a,#0x40	; transmission bit
	acall	spi_transfer
	; transmit argument
	mov	a,r3
	acall	spi_transfer
	mov	a,r2
	acall	spi_transfer
	mov	a,r1
	acall	spi_transfer
	mov	a,r0
	acall	spi_transfer
	; transmit crc
	mov	a,r4
	orl	a,#0x01	; end bit
	acall	spi_transfer
	pop	acc
	ret
