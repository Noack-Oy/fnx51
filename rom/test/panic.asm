
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
.include ../global/variables.equ
.include ../serial/sfr.equ

; *************
; * Main Code *
; *************

	.org	0

.include ../global/init.inc

; *** test/panic.asm ***

	acall	serial_init

	acall	panic

; *********************
; * Library Functions *
; *********************
.include ../serial/init.inc
.include ../serial/rx.inc
.include ../serial/tx.inc
.include ../print/char.inc
.include ../print/text.inc
.include ../print/hex.inc

.equ panic_out, serial_tx
.include ../util/panic.inc
