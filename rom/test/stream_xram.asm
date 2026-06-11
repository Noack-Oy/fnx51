
; ****************************
; * XRAM Stream Test Program *
; ****************************

; This program wtites and reads XRAM through a stream


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

; *** test/stream_xram.asm ***

	acall	serial_init

	push	auxr
	; use xram
	anl	auxr,#0xe1	; clear extram, xrs0-2
	orl	auxr,#0x10	; set xrs2 (size 1792 bytes)

	; clear xram with dummy value
	mov	dptr,#0
memory_init__1:
	mov	a,#0x24
	movx	@dptr,a
	inc	dptr
	mov	a,#0x07 ; xram goes up to 0x06ff
	cjne	a,dph,memory_init__1

	; disable xram again, stram_xram should enable it temporarily
	orl	auxr,#0x02	; set EXTRAM bit

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
.include ../serial/init.inc
.include ../serial/rx.inc
.include ../serial/tx.inc
.include ../stream/xram_read.inc
.include ../stream/xram_write.inc
.include ../print/char.inc
.include ../print/text.inc
.include ../print/hex.inc
.include ../print/int.inc
.include ../read/char.inc
.include ../util/dump.inc
.include ../util/xch.inc

.equ panic_out, serial_tx
.include ../util/panic.inc
