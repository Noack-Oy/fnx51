
; *** spi/transfer.inc ***

; >> char{a} serial_transfer(char{a} data) <<

; SPI transfer function: send byte from a, read response into a

spi_transfer:
	mov	spdat,a

spi_transfer__wait:
	mov	a,spsta
	anl	a,#0x80	;check SPIF bit
	jz	spi_transfer__wait

	mov	a,spdat
	ret
