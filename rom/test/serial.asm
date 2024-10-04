
; *********************************
; * Serial Interface Test Program *
; *********************************

; This program prints "Hello, world!"


; **********************
; * Header Definitions *
; **********************
.inc ../serial/sfr.equ


; *************
; * Main Code *
; *************

; *** test/serial.asm ***

	.org	0

	acall	serial_init

	mov	DPTR,	#hello_text
hello_loop:
	mov	a,	#0
	movc	a,	@DPTR+a
	jz	hello_end
	acall	serial_tx
	inc	dptr
	ajmp	hello_loop

hello_end:
	ajmp	*

hello_text:
	.byte	"Hello, world!"
	.byte	0


; *********************
; * Library Functions *
; *********************
.inc ../serial/init.inc
.inc ../serial/tx.inc
