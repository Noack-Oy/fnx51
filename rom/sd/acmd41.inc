
; *** sd/acmd41.inc ***

; >> char{a} sd_acmd41() <<

; ACMD41: send operation condition
sd_acmd41:
	acall	sd_cmd55
	cjne	a,#1,sd_acmd41__end
	acall	regbank_next
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,#0x40 ; indicates high capacity support
	mov	r4,a
	mov	a,#41
	acall	sd_command
	acall	sd_response1
	acall	regbank_prev
sd_acmd41__end:
	ret
