
; *** serial/rx.inc ***

; >> char serial_rx <<

; Receive byte into accumulator.

serial_rx:
	jnb	RI,	serial_rx
	clr	RI
	mov	a,	SBUF
	ret
