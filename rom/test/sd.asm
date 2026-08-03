
; ************************
; * SD Card Test Program *
; ************************

; This program initializes an SD card
; and dumps the first sector.


; **********************
; * Header Definitions *
; **********************
.include ../global/variables.equ
.include ../global/sfr.equ
.include ../serial/sfr.equ
.include ../spi/sfr.equ
.include ../sd/config.equ

; *************
; * Main Code *
; *************

; *** test/sd.asm ***

.org	0
.include	../global/init.inc

	acall	serial_init
	acall	sd_init
	acall	print_hex_8	; status
	acall	print_hex_32	; operation condition
	mov	a,#13
	acall	print_char
	mov	a,#10
	acall	print_char

	; use xram
	anl	auxr,#0xe1	; clear extram, xrs0-2
	orl	auxr,#0x10	; set xrs2 (size 1792 bytes)

	clr	a
	mov	dpl,a
	mov	dph,a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	acall	sd_block_read

	; print CRC
	mov	a,r4
	mov	r0,a
	mov	a,r5
	mov	r1,a
	acall	print_hex_16
	mov	a,#13
	acall	print_char
	mov	a,#10
	acall	print_char

	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	stream_in,a
	mov	stream_in+1,a
	mov	dptr,#stream_xram_read
	mov	in,dpl
	mov	in+1,dph
	acall	dump
	acall	dump

halt:
	sjmp	halt

.org	0x0100

; *********************
; * Library Functions *
; *********************
.include ../serial/init.inc
.include ../serial/tx.inc
.include ../serial/rx.inc

.include ../spi/transfer.inc
.include ../sd/init.inc
.include ../sd/warmup.inc
.include ../sd/select.inc
.include ../sd/cmd0.inc
.include ../sd/cmd8.inc
.include ../sd/acmd41.inc
.include ../sd/cmd55.inc
.include ../sd/cmd58.inc
.include ../sd/command.inc
.include ../sd/response.inc
.include ../sd/block_read.inc

.include ../util/delay.inc
.include ../print/char.inc
.include ../print/hex.inc
.include ../print/text.inc
.include ../stream/xram_read.inc
.include ../read/char.inc
.include ../util/xch.inc
.include ../util/dump.inc

.equ panic_out, serial_tx
.include ../util/panic.inc
