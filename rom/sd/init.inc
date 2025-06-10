
; *** sd/init.inc ***

; >> uint8{a},uint32{r0-r3} sd_init() <<

; Initialize SD card, return error code in a, OCR in r0-r3.
sd_init:
	mov	a,r5	; r5 is used as temp store for error code
	push	acc

	mov	spcon,#sd_spi_config
	acall	sd_warmup
	acall	sd_select

	mov	r5,#5	; 5: no idle response to cmd0
	acall	sd_cmd0
	cjne	a,#1,sd_init__end

	dec	r5	; 4: no idle response to cmd8
	acall	sd_cmd8
	cjne	a,#1,sd_init__end
	dec	r5	; 3: invalid cmd8 response
	cjne	r3,#0x00,sd_init__end
	cjne	r2,#0x00,sd_init__end
	cjne	r1,#0x01,sd_init__end
	cjne	r0,#0xaa,sd_init__end

	mov	r5,#100	; retry counter
sd_init__loop:
	acall	sd_acmd41
	jz	sd_init__continue
	mov	a,#10
	acall	delay_ms_8
	djnz	r5,sd_init__loop
	mov	r5,#2	; 2: acmd41 ready response timeout
	sjmp	sd_init__end

sd_init__continue:
	mov	r5,#1	; 1: no ready response to cmd58
	acall	sd_cmd58

	dec	r5	; 0: success

sd_init__end:
	pop	acc
	xch	a,r5
	ret