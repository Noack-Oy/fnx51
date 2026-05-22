
; *** sd/cmd58.inc ***

; >> uint8{a},uint32{r0-r3} sd_cmd58() <<

; CMD58: read operation conditions register
sd_cmd58:
	push	4
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	r4,a
	mov	a,#58
	acall	sd_command
	acall	sd_response3
	pop	4
	ret
