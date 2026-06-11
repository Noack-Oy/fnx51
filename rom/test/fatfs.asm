
; ***************************************
; * FAT File System Device Test Program *
; ***************************************

; This program tests the FAT file system implementation.


; **********************
; * Header Definitions *
; **********************
.include ../global/variables.equ
.include ../global/sfr.equ
.include ../serial/sfr.equ
.include ../spi/sfr.equ
.include ../sd/config.equ
.include ../fatfs/info.equ
.include ../fatfs/chain.equ

; *************
; * Main Code *
; *************

; *** test/fatfs.asm ***

.org	0
.include	../global/init.inc

	lcall	serial_init
	lcall	memory_init
	lcall	sd_init
	jz	__1
	lcall	panic ; sd init error
__1:
	lcall	block_init

	mov	dptr,#ok_string
	lcall	print_text

	; this is where the interesting part starts
	lcall	fatfs_init

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
	lcall	dump

	mov	dptr,#newline
	lcall	print_text

	; Load root directory cluster (from fatfs_info+17) into r0-r3
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	mov	a,#fatfs_info_root_dir
	lcall	dptr_index
	; now dptr points to fatfs_info + offset 17 (root directory cluster)
	lcall	dptr_read_32
	; r0-r3 now contains the root cluster number - print it to verify
	lcall	print_hex_32
	mov	dptr,#newline
	lcall	print_text
	; now call cluster_to_lba with this value
	lcall	fatfs_cluster_to_lba
	lcall	print_hex_32
	mov	dptr,#newline
	lcall	print_text

	; *** chain handle exercise ***
	; open a chain on the root directory cluster, ensure block 0
	; (the first sector of the root dir) is loaded, dump the first
	; 32 bytes of it (one directory entry), close the chain.

	; reload the root cluster number into r0-r3
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	mov	a,#fatfs_info_root_dir
	lcall	dptr_index
	lcall	dptr_read_32

	; open chain; dptr = handle on return
	lcall	fatfs_chain_open

	; ensure block 0 of the chain is loaded. ensure_block preserves
	; the handle in dptr and returns the borrowed cached pointer in
	; dptr'.
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	lcall	fatfs_chain_ensure_block

	; Read cached pointer out of dptr' into stream_in, then reuse
	; the same slot to load the read helper for dump. dptr (handle)
	; survives throughout.
	inc	auxr1
	mov	stream_in,dpl
	mov	stream_in+1,dph
	mov	dptr,#stream_xram_read
	mov	in,dpl
	mov	in+1,dph
	inc	auxr1

	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	a,#32
	lcall	dump

	; print_text wants its string pointer in the active dptr; park
	; the immediate in dptr' so the handle in dptr survives.
	inc	auxr1
	mov	dptr,#newline
	lcall	print_text
	inc	auxr1

	lcall	fatfs_chain_close

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
.include ../memory/release.inc
.include ../block/init.inc
.include ../block/load.inc
.include ../block/flush.inc

.include ../fatfs/init.inc
.include ../fatfs/cluster.inc
.include ../fatfs/chain.inc

.include ../math/arith32.inc

.include ../util/delay.inc
.include ../util/xch.inc
.include ../util/dptr.inc

.include ../print/char.inc
.include ../print/hex.inc
.include ../print/text.inc

.include ../stream/xram_read.inc
.include ../read/char.inc
.include ../util/dump.inc

.equ panic_out, serial_tx
.include ../util/panic.inc
