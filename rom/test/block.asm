
; *****************************
; * Block Device Test Program *
; *****************************

; This program uses the block device layer
; to read data from an SD card.


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

; *** test/block.asm ***

.org	0
.inc	../global/init.inc

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

.inc ../memory/init.inc
.inc ../memory/allocate.inc
.inc ../block/init.inc
.inc ../block/load.inc
.inc ../block/flush.inc

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
