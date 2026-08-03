
; *****************************
; * Block Device Test Program *
; *****************************

; This program uses the block device layer
; to read data from an SD card.


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

; *** test/block.asm ***

.org	0
.include	../global/init.inc

	acall	serial_init
	acall	memory_init
	acall	sd_init
	jz	__1
	acall	panic ; sd init error
__1:
	acall	block_init

	mov	dptr,#ok_string
	acall	print_text

	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	acall	block_load

	; dump whole first sector
	mov	stream_in,dpl
	mov	stream_in+1,dph
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	dptr,#stream_xram_read
	mov	in,dpl
	mov	in+1,dph
	acall	dump
	acall	dump
	mov	r0,stream_in
	mov	r1,stream_in+1
	dec	r1
	dec	r1

	mov	dptr,#newline
	acall	print_text

	; dump partition table
	mov	dptr,#446
	mov	a,r0
	add	a,dpl
	mov	stream_in,a
	mov	a,r1
	addc	a,dph
	mov	stream_in+1,a
	clr	a
	mov	r0,a
	mov	r1,a
	mov	a,#4*16
	acall	dump

	mov	dptr,#newline
	acall	print_text

	; get lba of first partition
	mov	a,stream_in
	clr	c
	subb	a,#4*16-8
	mov	dpl,a
	mov	a,stream_in+1
	subb	a,#0
	mov	dph,a
	movx	a,@dptr
	inc	dptr
	mov	r0,a
	movx	a,@dptr
	inc	dptr
	mov	r1,a
	movx	a,@dptr
	inc	dptr
	mov	r2,a
	movx	a,@dptr
	inc	dptr
	mov	r3,a

	acall	print_hex_32
	mov	dptr,#newline
	acall	print_text

	; load and dump volume id sector
	acall	block_load
	mov	stream_in,dpl
	mov	stream_in+1,dph
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	dptr,#stream_xram_read
	mov	in,dpl
	mov	in+1,dph
	acall	dump
	acall	dump

	mov	dptr,#newline
	acall	print_text


halt:
	sjmp	halt

ok_string:
	.db	"OK" 
newline: 
	.db	13, 10, 0

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

.include ../memory/init.inc
.include ../memory/allocate.inc
.include ../block/init.inc
.include ../block/load.inc
.include ../block/flush.inc

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
