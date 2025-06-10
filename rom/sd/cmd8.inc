
; *** sd/cmd8.inc ***

; >> uint8{a},uint32{r0-r3} sd_cmd8() <<

; CMD8: send interface condition
sd_cmd8:
	mov	a,r4
	push	acc
	mov	a,#8
	mov	r0,#0xaa
	mov	r1,#0x01
	mov	r2,#0x00
	mov	r3,#0x00
	mov	r4,#0x86
	acall	sd_command
	acall	sd_response7
	mov	r4,a
	pop	acc
	xch	a,r4
	ret
