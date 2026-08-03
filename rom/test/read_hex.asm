
; ************************************
; * Hex Integer Parsing Test Program *
; ************************************

; Parses and prints back hex numbers in a loop.


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

; *** test/read_int_hex.asm ***

	acall	serial_init

loop:
	acall	read_hex_32
	acall	print_hex_32
	sjmp	loop


; *********************
; * Library Functions *
; *********************
.include ../serial/init.inc
.include ../serial/tx.inc
.include ../serial/rx.inc
.include ../read/char.inc
.include ../read/hex.inc
.include ../print/char.inc
.include ../print/hex.inc
