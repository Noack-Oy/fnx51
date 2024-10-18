
; ************************
; * Hexdump Test Program *
; ************************

; This program reads 37 bytes and
; echoes them in hexdump format


; **********************
; * Header Definitions *
; **********************
.inc ../global/variables.equ
.inc ../serial/sfr.equ


; *************
; * Main Code *
; *************

.org 0
.inc ../global/init.inc

; *** test/dump.asm ***

	acall	serial_init

	clr	a
	mov	r0,	a
	mov	r1,	a
	mov	r2,	a
	mov	r3,	a
	mov	a,	#37

	acall	dump
	acall	print_hex_32
	mov	a,	#13
	acall	print_char
	mov	a,	#10
	acall	print_char

	sjmp	*


; *********************
; * Library Functions *
; *********************
.inc ../serial/init.inc
.inc ../serial/rx.inc
.inc ../serial/tx.inc
.inc ../print/char.inc
.inc ../print/hex.inc
.inc ../read/char.inc
.inc ../util/dump.inc
.inc ../util/xch.inc
