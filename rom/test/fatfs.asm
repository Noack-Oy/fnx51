
; ***************************************
; * FAT File System Device Test Program *
; ***************************************

; This program tests the FAT file system implementation.


; **********************
; * Header Definitions *
; **********************
.inc ../global/variables.equ
.inc ../global/sfr.equ
.inc ../serial/sfr.equ
.inc ../spi/sfr.equ
.inc ../sd/config.equ
.inc ../fatfs/info.equ

; *************
; * Main Code *
; *************

; *** test/fatfs.asm ***

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

	; this is where the interesting part starts
	acall	fatfs_init

	; dump fatfs_info data structure

	mov	stream_in,fatfs_info
	mov	stream_in+1,fatfs_info+1
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	dptr,#stream_xram_read
	mov	in,dpl
	mov	in+1,dph
	mov	a,#fatfs_info_size
	acall	dump

	mov	dptr,#newline
	acall	print_text

	; Load root directory cluster (from fatfs_info+17) into r0-r3
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	mov	a,#fatfs_info_root_dir
	acall	dptr_index
	; now dptr points to fatfs_info + offset 17 (root directory cluster)
	acall	dptr_read_32
	; r0-r3 now contains the root cluster number - print it to verify
	acall	print_hex_32
	mov	dptr,#newline
	acall	print_text
	; now call cluster_to_lba with this value
	acall	fatfs_cluster_to_lba
	acall	print_hex_32
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

.inc ../fatfs/init.inc
.inc ../fatfs/cluster.inc

.inc ../math/arith32.inc

.inc ../util/delay.inc
.inc ../util/regbank.inc
.inc ../util/xch.inc
.inc ../util/dptr.inc

.inc ../print/char.inc
.inc ../print/hex.inc
.inc ../print/text.inc

.inc ../stream/xram_read.inc
.inc ../read/char.inc
.inc ../util/dump.inc

.equ panic_out, serial_tx
.inc ../util/panic.inc
