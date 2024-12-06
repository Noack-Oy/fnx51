
; ************************************
; * Memory Management Test Program *
; ************************************

; Interactive test:
; d <addr> <size>; 	- dump memory
; g <size>;		- getmem
; f <addr> <size>;	- freemem


; **********************
; * Header Definitions *
; **********************
.inc ../global/variables.equ
.inc ../global/sfr.equ
.inc ../serial/sfr.equ


; *************
; * Main Code *
; *************

.org 0
.inc ../global/init.inc

; *** test/memory.asm ***

	acall	serial_init

	; use xram
	mov	a,	auxr
	anl	a,	0xe1	; clear extram, xrs0-2
	orl	a,	0x10	; set xrs2 (size 1792 bytes)
	mov	auxr,	a


loop:
	acall	read_char

	cjne	a,	#'d',	__1
	acall	read_hex_32
	mov	stream_in,	r0
	mov	stream_in+1,	r1
	acall	read_hex_32
	mov	a,	r0
	mov	r0,	stream_in
	mov	r1,	stream_in+1
	mov	r2,	#0
	mov	r3,	#0
	acall	dump
	sjmp	loop
__1:
	cjne	a,	#'g',	__2
	acall	read_hex_32
	acall	getmem
	acall	print_hex_32
	sjmp	loop
__2:
	cjne	a,	#'f',	__3
	acall	read_hex_32
	mov	a,	r0
	mov	r4,	a
	mov	a,	r1
	mov	r5,	a
	acall	read_hex_32
	mov	a,	r0
	mov	r2,	a
	mov	a,	r1
	mov	r3,	a
	mov	a,	r4
	mov	r0,	a
	mov	a,	r5
	mov	r1,	a
	acall	freemem
__3:
	mov	a,	#'?'
	acall	print_char
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
.inc ../print/text.inc
.inc ../print/hex.inc
.inc ../util/xch.inc
.inc ../util/regbank.inc
.inc ../util/dump.inc
.inc ../memory/getmem.inc
.inc ../memory/freemem.inc

.equ panic_out, serial_tx
.inc ../util/panic.inc
