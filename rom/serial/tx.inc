
; *** serial/tx.inc ***

; >> void serial_tx(char a) <<

; Transmit byte from accumulator.

serial_tx:
	jnb	TI,	serial_tx
	clr	TI
	mov	SBUF,	a
	ret
