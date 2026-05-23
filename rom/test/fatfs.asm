
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
.inc ../fatfs/chain.equ

; iram scratch local to this test program (just below the stack at 0x60)
.equ	test_name_buf,		0x5A	; ..0x5B, name buffer pointer in xram
.equ	test_dst_buf,		0x5C	; ..0x5D, file_read dst buffer pointer
.equ	test_file_handle,	0x5E	; ..0x5F, file handle pointer

; *************
; * Main Code *
; *************

; *** test/fatfs.asm ***

.org	0
.inc	../global/init.inc

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

	; *** dir iterator exercise ***
	; open a dir iterator on the root cluster, dump each short-name
	; entry until end-of-directory, close.

	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	mov	a,#fatfs_info_root_dir
	lcall	dptr_index
	lcall	dptr_read_32
	lcall	fatfs_dir_open		; dptr = dir handle

dir_loop:
	; dir_next preserves dptr (handle in dptr), returns status in a
	; and entry pointer in dptr'.
	lcall	fatfs_dir_next
	jnz	dir_done

	; Swap dptr roles so the dump / print_text below can clobber the
	; active dptr freely while the handle hides in dptr'. dir_next
	; is contractually required to preserve dptr', so we'll get the
	; handle back with a second swap before looping.
	inc	auxr1
	mov	stream_in,dpl		; (active = entry buffer pointer)
	mov	stream_in+1,dph

	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	dptr,#stream_xram_read
	mov	in,dpl
	mov	in+1,dph
	mov	a,#32
	lcall	dump

	mov	dptr,#newline
	lcall	print_text

	inc	auxr1			; swap back: active = handle
	sjmp	dir_loop

dir_done:
	lcall	fatfs_dir_close

	; *** file open/read/close exercise ***
	; allocate an 11-byte xram buffer, write "ALICE'~1TXT" into it,
	; open the file, read 32 bytes into a 32-byte xram buffer, dump,
	; close.

	; allocate 11-byte name buffer
	mov	r0,#11
	mov	r1,#0
	lcall	memory_allocate
	mov	test_name_buf,dpl
	mov	test_name_buf+1,dph

	; write "ALICE'~1TXT" (11 bytes) to the buffer
	mov	a,#'A'
	movx	@dptr,a
	inc	dptr
	mov	a,#'L'
	movx	@dptr,a
	inc	dptr
	mov	a,#'I'
	movx	@dptr,a
	inc	dptr
	mov	a,#'C'
	movx	@dptr,a
	inc	dptr
	mov	a,#'E'
	movx	@dptr,a
	inc	dptr
	mov	a,#'\''
	movx	@dptr,a
	inc	dptr
	mov	a,#'~'
	movx	@dptr,a
	inc	dptr
	mov	a,#'1'
	movx	@dptr,a
	inc	dptr
	mov	a,#'T'
	movx	@dptr,a
	inc	dptr
	mov	a,#'X'
	movx	@dptr,a
	inc	dptr
	mov	a,#'T'
	movx	@dptr,a

	; allocate 32-byte dst buffer
	mov	r0,#32
	mov	r1,#0
	lcall	memory_allocate
	mov	test_dst_buf,dpl
	mov	test_dst_buf+1,dph

	; file_open(name in dptr)
	mov	dpl,test_name_buf
	mov	dph,test_name_buf+1
	lcall	fatfs_file_open
	; dptr = file handle, a = status
	jz	file_open_ok
	lcall	panic	; file not found
file_open_ok:

	; remember file handle
	mov	test_file_handle,dpl
	mov	test_file_handle+1,dph

	; file_read(handle in dptr, n=32 in a, dst in dptr')
	inc	auxr1
	mov	dpl,test_dst_buf
	mov	dph,test_dst_buf+1
	inc	auxr1
	mov	a,#32
	lcall	fatfs_file_read
	; a = bytes read (should be 32)

	; dump 32 bytes from dst buffer
	mov	stream_in,test_dst_buf
	mov	stream_in+1,test_dst_buf+1
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	dptr,#stream_xram_read
	mov	in,dpl
	mov	in+1,dph
	mov	a,#32
	lcall	dump

	mov	dptr,#newline
	lcall	print_text

	; close file
	mov	dpl,test_file_handle
	mov	dph,test_file_handle+1
	lcall	fatfs_file_close

halt:
	sjmp	halt

ok_string:
	.db	"OK" 
newline: 
	.db	13, 10, 0

.org	0x0200

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
.inc ../memory/release.inc
.inc ../block/init.inc
.inc ../block/load.inc
.inc ../block/flush.inc

.inc ../fatfs/init.inc
.inc ../fatfs/cluster.inc
.inc ../fatfs/chain.inc
.inc ../fatfs/dir_iter.inc
.inc ../fatfs/file.inc

.inc ../math/arith32.inc

.inc ../util/delay.inc
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
