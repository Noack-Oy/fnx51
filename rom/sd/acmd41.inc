
; *** sd/acmd41.inc ***

; >> char{a} sd_acmd41() <<

; ACMD41: send operation condition
sd_acmd41:
	acall	sd_cmd55
	cjne	a,#1,sd_acmd41__end
	push	0
	push	1
	push	2
	push	3
	push	4
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,#0x40 ; indicates high capacity support
	mov	r4,a
	mov	a,#41
	acall	sd_command
	acall	sd_response1
	pop	4
	pop	3
	pop	2
	pop	1
	pop	0
sd_acmd41__end:
	ret
