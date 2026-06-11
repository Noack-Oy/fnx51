
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
.include ../global/variables.equ
.include ../global/sfr.equ
.include ../serial/sfr.equ


; *************
; * Main Code *
; *************

.org 0
.include ../global/init.inc

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
	mov	r0,dpl		; print_hex_16 reads from r0r1
	mov	r1,dph
	acall	print_hex_16
	sjmp	next
__2:
	cjne	a,#'R',__3
	acall	read_hex_32	; addr in r0r1; stash in dptr (preserved by next call)
	mov	dpl,r0
	mov	dph,r1
	acall	read_hex_32	; size in r0r1
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
.include ../serial/init.inc
.include ../serial/tx.inc
.include ../serial/rx.inc
.include ../read/char.inc
.include ../read/hex.inc
.include ../print/char.inc
.include ../print/text.inc
.include ../print/hex.inc
.include ../util/xch.inc
.include ../util/dump.inc
.include ../stream/xram_read.inc
.include ../memory/init.inc
.include ../memory/allocate.inc
.include ../memory/release.inc

.equ panic_out, serial_tx
.include ../util/panic.inc
