
; *** sd/block_read.inc ***

; >> uint16{r4-r5} sd_block_read(
;	uint32{r0-r3} &block_address,
;	char *{dptr}&destination) <<

; Read block at address from sd card, write to RAM at dptr.
; Return CRC in r4-r5, address incremented,
; dptr pointing to first byte after read block.

sd_block_read:
	push	acc

	; send CMD17 (single block read)
	mov	a,#17
	mov	r4,#0xff ; command CRC, ignored
	acall	sd_command
sd_block_read__wait:
	acall	sd_response1
	cjne	a,#0xfe,sd_block_read__wait
	mov	r5,#0 ; loop counter
	; this loop will run 256 times,
	; two bytes are read per iteration
sd_block_read__loop:
	mov	a,#0xff
	acall	spi_transfer
	movx	@dptr,a
	inc	dptr
	mov	a,#0xff
	acall	spi_transfer
	movx	@dptr,a
	inc	dptr
	djnz	r5,sd_block_read__loop
	; read block CRC
	mov	a,#0xff
	acall	spi_transfer
	mov	r5,a
	mov	a,#0xff
	acall	spi_transfer
	mov	r4,a
	; increment block address
	mov	a,#1
	add	a,r0
	mov	r0,a
	clr	a
	addc	a,r1
	mov	r1,a
	clr	a
	addc	a,r2
	mov	r2,a
	clr	a
	addc	a,r3
	mov	r3,a

	pop	acc
	ret
