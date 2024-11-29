
; *******************************
; * Panic function Test Program *
; *******************************

; This program triggers the "panic" function,
; which handles unrecoverable errors
; by printng the address from where it was called
; and halting the system.

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

; *** test/panic.asm ***

	acall	serial_init

	acall	panic

; *********************
; * Library Functions *
; *********************
.inc ../serial/init.inc
.inc ../serial/rx.inc
.inc ../serial/tx.inc
.inc ../print/char.inc
.inc ../print/text.inc
.inc ../print/hex.inc

.equ panic_out, serial_tx
.inc ../util/panic.inc
