
; ************************************
; * Hex Integer Parsing Test Program *
; ************************************

; Parses and prints back hex numbers in a loop.


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

; *** test/read_int_hex.asm ***

	acall	serial_init

loop:
	acall	read_hex_32
	acall	print_hex_32
	sjmp	loop


; *********************
; * Library Functions *
; *********************
.inc ../serial/init.inc
.inc ../serial/tx.inc
.inc ../serial/rx.inc
.inc ../read/char.inc
.inc ../read/hex.inc
.inc ../print/char.inc
.inc ../print/hex.inc
