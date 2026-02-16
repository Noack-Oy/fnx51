
; *** util/delay.inc ***

; >> void delay_ms_8(uint8{a} duration) <<

; 1-256 millisecond delay, duration given in a.
; Assumes 20MHz clock and X2 mode.
; Actual delay is slightly longer.
delay_ms_8:

	push	acc
	push	b

	; This loop is repeated a times (256 if a is zero).
	; The goal is to stretch each repeat to one millisecond.
delay_ms_8__loop1:
	; Each millisecond is 20k clock periods.
	; In X2 mode, a machine cycle is 6 clock periods.
	; Therefore, a millisecond is approx. 3334 machine cycles.
	mov	b,#185	; n = 185	cycles:	  2
delay_ms_8__loop2:
	push	acc			; n*2 = 370
	push	b			; n*2 = 370
	mul	ab			; n*4 = 740
	mul	ab			; n*4 = 740
	pop	b			; n*2 = 370
	pop	acc			; n*2 = 370
	djnz	b,delay_ms_8__loop2	; n*2 = 370
	djnz	acc,delay_ms_8__loop1	;	  2
					; Sum: 3334
	pop	b
	pop	acc
	ret
