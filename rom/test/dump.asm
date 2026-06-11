
; ************************
; * Hexdump Test Program *
; ************************

; This program reads 37 bytes and
; echoes them in hexdump format


; **********************
; * Header Definitions *
; **********************
.include ../global/variables.equ
.include ../serial/sfr.equ


; *************
; * Main Code *
; *************

.org 0
.include ../global/init.inc

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
.include ../serial/init.inc
.include ../serial/rx.inc
.include ../serial/tx.inc
.include ../print/char.inc
.include ../print/hex.inc
.include ../read/char.inc
.include ../util/dump.inc
.include ../util/xch.inc
