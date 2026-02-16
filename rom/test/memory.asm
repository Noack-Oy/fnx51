
; ************************************
; * Memory Management Test Program *
; ************************************

; Interactive test:
; D <addr> <size>; 	- dump memory
; A <size>;		- allocate memory
; R <addr> <size>;	- release memory


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
	acall	memory_init
loop:
	acall	read_char

	cjne	a,#'D',__1
	acall	read_hex_32
	mov	stream_in,r0
	mov	stream_in+1,r1
	acall	read_hex_32
	mov	a,r0
	mov	r0,stream_in
	mov	r1,stream_in+1
	mov	r2,#0
	mov	r3,#0
	push	in
	push	in+1
	mov	dptr,#stream_xram_read
	mov	in,dpl
	mov	in+1,dph
	acall	dump
	pop	in+1
	pop	in
	sjmp	loop
__1:
	cjne	a,#'A',__2
	acall	read_hex_32
	acall	memory_allocate
	acall	print_hex_16
	sjmp	next
__2:
	cjne	a,#'R',__3
	acall	read_hex_32
	mov	a,r0
	mov	r4,a
	mov	a,r1
	mov	r5,a
	acall	read_hex_32
	mov	a,r0
	mov	r2,a
	mov	a,r1
	mov	r3,a
	mov	a,r4
	mov	r0,a
	mov	a,r5
	mov	r1,a
	acall	memory_release
	sjmp	next
__3:
	mov	a,#'?'
	acall	print_char
next:
	mov	a,#13
	acall	print_char
	mov	a,#10
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
.inc ../stream/xram_read.inc
.inc ../memory/init.inc
.inc ../memory/allocate.inc
.inc ../memory/release.inc

.equ panic_out, serial_tx
.inc ../util/panic.inc
