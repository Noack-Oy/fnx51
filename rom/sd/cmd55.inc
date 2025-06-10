
; *** sd/cmd55.inc ***

; >> char{a} sd_cmd55() <<

; CMD55: application specific command (prefix)
sd_cmd55:
	acall	regbank_next
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	r4,a
	mov	a,#55
	acall	sd_command
	acall	sd_response1
	acall	regbank_prev
	ret
