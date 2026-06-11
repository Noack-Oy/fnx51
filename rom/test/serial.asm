
; *********************************
; * Serial Interface Test Program *
; *********************************

; This program prints "Hello, world!",
; then echoes every byte received,
; but incremented by one


; **********************
; * Header Definitions *
; **********************
.include ../global/variables.equ
.include ../serial/sfr.equ


; *************
; * Main Code *
; *************

	.org	0

.include ../global/init.inc

; *** test/serial.asm ***

	acall	serial_init

	mov	dptr,	#hello_text
hello_loop:
	clr	a
	movc	a,	@a+dptr
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
.include ../serial/init.inc
.include ../serial/tx.inc
.include ../serial/rx.inc
