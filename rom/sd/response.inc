
; *** sd/response.inc ***

; >> uint8{a} sd_response1() <<

; Read response from SD card in 'R1' format into a
sd_response1:
	push	b
	mov	b,8	; number of tries
sd_response1__loop:
	mov	a,#0xff
	acall	spi_transfer
	cjne	a,#0xff,sd_response1__end	; success
	djnz	b,sd_response1__loop	; retry / timeout
sd_response1__end:
	pop	b
	ret

; >> uint8{a}, uint32{r0-r3} sd_response3() <<

; Read response from SD card in 'R3' format into a and r0-r3
sd_response3: ; same as R7

; >> uint8{a}, uint32{r0-r3} sd_response7() <<

; Read response from SD card in 'R7' format into a and r0-r3
sd_response7:
	acall	sd_response1
	cjne	a,#0x02,sd_response7__1
sd_response7__1:
	jc	sd_response7__2
	ret	; error in R1 response
sd_response7__2:
	mov	r0,a
	mov	a,#0xff
	acall	spi_transfer
	mov	r3,a
	mov	a,#0xff
	acall	spi_transfer
	mov	r2,a
	mov	a,#0xff
	acall	spi_transfer
	mov	r1,a
	mov	a,#0xff
	acall	spi_transfer
	xch	a,r0
	ret
