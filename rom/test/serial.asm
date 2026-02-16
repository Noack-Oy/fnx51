
; *********************************
; * Serial Interface Test Program *
; *********************************

; This program prints "Hello, world!",
; then echoes every byte received,
; but incremented by one


; **********************
; * Header Definitions *
; **********************
.inc ../global/variables.equ
.inc ../serial/sfr.equ


; *************
; * Main Code *
; *************

	.org	0

.inc ../global/init.inc

; *** test/serial.asm ***

	acall	serial_init

	mov	dptr,	#hello_text
hello_loop:
	clr	a
	movc	a,	@dptr+a
	jz	hello_end
	acall	serial_tx
	inc	dptr
	sjmp	hello_loop
hello_end:

echo_loop:
	acall	serial_rx
	inc	a
	acall	serial_tx
	sjmp	echo_loop

hello_text:
	.byte	"Hello, world!"
	.byte	13, 10, 0


; *********************
; * Library Functions *
; *********************
.inc ../serial/init.inc
.inc ../serial/tx.inc
.inc ../serial/rx.inc
