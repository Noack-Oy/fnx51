
; ************************
; * SD Card Test Program *
; ************************

; This program initializes an SD card
; and dumps the first sector.


; **********************
; * Header Definitions *
; **********************
.inc ../global/variables.equ
.inc ../global/sfr.equ
.inc ../serial/sfr.equ
.inc ../spi/sfr.equ
.inc ../sd/config.equ

; *************
; * Main Code *
; *************

; *** test/sd.asm ***

	.org	0
.inc	../global/init.inc

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

; *********************
; * Library Functions *
; *********************
.inc ../serial/init.inc
.inc ../serial/tx.inc
.inc ../serial/rx.inc

.inc ../spi/transfer.inc
.inc ../sd/init.inc
.inc ../sd/warmup.inc
.inc ../sd/select.inc
.inc ../sd/cmd0.inc
.inc ../sd/cmd8.inc
.inc ../sd/acmd41.inc
.inc ../sd/cmd55.inc
.inc ../sd/cmd58.inc
.inc ../sd/command.inc
.inc ../sd/response.inc
.inc ../sd/block_read.inc

.inc ../util/delay.inc
.inc ../util/regbank.inc
.inc ../print/char.inc
.inc ../print/hex.inc
.inc ../print/text.inc
.inc ../stream/xram_read.inc
.inc ../read/char.inc
.inc ../util/xch.inc
.inc ../util/dump.inc

.equ panic_out, serial_tx
.inc ../util/panic.inc
