
; *** sd/cmd0.inc ***

; >> char{a} sd_cmd0() <<

; CMD0: go to idle state
sd_cmd0:
	push	0
	push	1
	push	2
	push	3
	push	4
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	r4,#0x94
	acall	sd_command
	acall	sd_response1
	pop	4
	pop	3
	pop	2
	pop	1
	pop	0
	ret
