
; ****************************
; * XRAM Stream Test Program *
; ****************************

; This program wtites and reads XRAM through a stream


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

; *** test/stream.asm ***

	acall	serial_init

.equ	scratch,	0x180

	push	out
	push	out+1
	mov	dptr,	#stream_xram_write
	mov	out,	dpl
	mov	out+1,	dph
	mov	dptr,	#scratch
	mov	stream_out,	dpl
	mov	stream_out+1,	dph
	mov	dptr,	#test_message
	acall	print_text
	mov	r0,	stream_out
	mov	r1,	stream_out+1
	acall	print_int_u16
	pop	out+1
	pop	out

	clr	a
	mov	r3,	a
	mov	r2,	a
	mov	r1,	a
	mov	r0,	a
	mov	stream_in,	a
	mov	stream_in+1,	a
	mov	dptr,	#stream_xram_read
	mov	in,	dpl
	mov	in+1,	dph
	acall	dump
	acall	dump

	sjmp	*

test_message:
	.db	"Hello, XRAM: "
test_message_end:
	.db	0


; *********************
; * Library Functions *
; *********************
.inc ../serial/init.inc
.inc ../serial/rx.inc
.inc ../serial/tx.inc
.inc ../stream/xram_read.inc
.inc ../stream/xram_write.inc
.inc ../print/char.inc
.inc ../print/text.inc
.inc ../print/hex.inc
.inc ../print/int.inc
.inc ../read/char.inc
.inc ../util/dump.inc
.inc ../util/xch.inc
.inc ../util/regbank.inc

.equ panic_out, serial_tx
.inc ../util/panic.inc
