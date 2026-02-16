
; *** sd/cmd0.inc ***

; >> char{a} sd_cmd0() <<

; CMD0: go to idle state
sd_cmd0:
	acall	regbank_next
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	r4,#0x94
	acall	sd_command
	acall	sd_response1
	acall	regbank_prev
	ret
