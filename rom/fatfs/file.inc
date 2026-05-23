
; *** fatfs/file.inc ***

; File handle. Extends the chain handle: chain header at offset 0,
; then size and current byte offset. One allocation.
;
; Layout (20 bytes):
;   bytes 0..11  chain handle (first cluster, current cluster, idx)
;   bytes 12..15 file size in bytes (uint32, from the dir entry)
;   bytes 16..19 current byte offset into the file (uint32)
;
; The 8.3 name passed to fatfs_file_open lives in xram — it must be
; 11 bytes, space-padded. Callers reading a name from the serial
; port place it there directly; the test program writes a literal
; into xram before calling.
;
; Internal iram scratch. Distinct from fatfs_chain_scratch_handle
; (0x52..0x53), which ensure_block uses during nested calls.

.equ	fatfs_file_scratch_name,	0x54	; ..0x55  name pointer
.equ	fatfs_file_scratch_dir,		0x56	; ..0x57  dir-iter handle
.equ	fatfs_file_scratch_file,	0x58	; ..0x59  file handle
.equ	fatfs_file_scratch_dst,		0x5A	; ..0x5B  dst pointer (file_read)

.equ	fatfs_file_size_off,		12	; ..15
.equ	fatfs_file_offset_off,		16	; ..19
.equ	fatfs_file_size,		20


; >> char *{dptr} fatfs_file_open(char *{dptr} name) → status{a} <<

; Walk the root directory looking for an entry whose 11-byte 8.3 name
; matches the 11 bytes pointed to by `name` (in xram). On match,
; allocate a file handle, return its pointer in dptr with status 0.
; If no entry matches, status is 1 and dptr is unspecified.

fatfs_file_open:
	push	b
	push	0
	push	1
	push	2
	push	3
	push	4
	push	5
	push	6
	push	7

	mov	fatfs_file_scratch_name,dpl
	mov	fatfs_file_scratch_name+1,dph

	; open a dir iterator on the root cluster
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	mov	a,#fatfs_info_root_dir
	lcall	dptr_index
	lcall	dptr_read_32
	lcall	fatfs_dir_open		; dptr = dir handle

	mov	fatfs_file_scratch_dir,dpl
	mov	fatfs_file_scratch_dir+1,dph

fatfs_file_open__loop:
	mov	dpl,fatfs_file_scratch_dir
	mov	dph,fatfs_file_scratch_dir+1
	lcall	fatfs_dir_next		; status in a, dptr' = entry buffer
	jz	fatfs_file_open__ok
	ljmp	fatfs_file_open__not_found
fatfs_file_open__ok:

	; compare entry (dptr') with name (in iram). Make dptr = name;
	; dptr' is already entry buffer.
	mov	dpl,fatfs_file_scratch_name
	mov	dph,fatfs_file_scratch_name+1
	mov	r0,#11
fatfs_file_open__cmp_loop:
	movx	a,@dptr
	mov	b,a
	inc	dptr
	inc	auxr1
	movx	a,@dptr
	inc	dptr
	inc	auxr1
	xrl	a,b
	jnz	fatfs_file_open__loop	; mismatch — try next entry
	djnz	r0,fatfs_file_open__cmp_loop

	; 11-byte match — pull first_cluster (entry[26..27] low,
	; entry[20..21] high) and file size (entry[28..31]).
	mov	dpl,fatfs_file_scratch_dir
	mov	dph,fatfs_file_scratch_dir+1
	mov	a,#fatfs_dir_buffer+26
	lcall	dptr_index
	movx	a,@dptr
	mov	r0,a
	inc	dptr
	movx	a,@dptr
	mov	r1,a

	mov	dpl,fatfs_file_scratch_dir
	mov	dph,fatfs_file_scratch_dir+1
	mov	a,#fatfs_dir_buffer+20
	lcall	dptr_index
	movx	a,@dptr
	mov	r2,a
	inc	dptr
	movx	a,@dptr
	mov	r3,a
	; r0-r3 = first_cluster

	push	0
	push	1
	push	2
	push	3
	mov	r0,#fatfs_file_size
	mov	r1,#0
	lcall	memory_allocate
	mov	fatfs_file_scratch_file,dpl
	mov	fatfs_file_scratch_file+1,dph
	pop	3
	pop	2
	pop	1
	pop	0
	lcall	fatfs_chain_init

	; copy size from entry[28..31] into handle[12..15]
	mov	dpl,fatfs_file_scratch_dir
	mov	dph,fatfs_file_scratch_dir+1
	mov	a,#fatfs_dir_buffer+28
	lcall	dptr_index
	lcall	dptr_read_32		; r0-r3 = size

	mov	dpl,fatfs_file_scratch_file
	mov	dph,fatfs_file_scratch_file+1
	mov	a,#fatfs_file_size_off
	lcall	dptr_index
	lcall	dptr_write_32

	; zero handle[16..19] (offset)
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	dpl,fatfs_file_scratch_file
	mov	dph,fatfs_file_scratch_file+1
	mov	a,#fatfs_file_offset_off
	lcall	dptr_index
	lcall	dptr_write_32

	; close dir iter
	mov	dpl,fatfs_file_scratch_dir
	mov	dph,fatfs_file_scratch_dir+1
	lcall	fatfs_dir_close

	mov	dpl,fatfs_file_scratch_file
	mov	dph,fatfs_file_scratch_file+1
	clr	a

	pop	7
	pop	6
	pop	5
	pop	4
	pop	3
	pop	2
	pop	1
	pop	0
	pop	b
	ret

fatfs_file_open__not_found:
	mov	dpl,fatfs_file_scratch_dir
	mov	dph,fatfs_file_scratch_dir+1
	lcall	fatfs_dir_close
	mov	a,#1
	pop	7
	pop	6
	pop	5
	pop	4
	pop	3
	pop	2
	pop	1
	pop	0
	pop	b
	ret


; >> void fatfs_file_close(char *{dptr} handle) <<

; Deinitialize the chain prefix and free the file handle.

fatfs_file_close:
	push	acc
	push	0
	push	1

	lcall	fatfs_chain_deinit
	mov	r0,#fatfs_file_size
	mov	r1,#0
	lcall	memory_release

	pop	1
	pop	0
	pop	acc
	ret


; >> uint8{a} fatfs_file_read(char *{dptr} handle, uint8{a} n, char *{dptr'} dst) <<

; Read up to `n` bytes from the current file position into `dst` and
; advance the offset by the same amount. Returns the actual count
; in `a`.
;
; Per-call cap: at most `min(n, bytes-left-in-current-block,
; bytes-left-in-file)` bytes are copied. Callers wanting more invoke
; file_read repeatedly. Cluster boundaries inside the chain are
; handled by ensure_block transparently.

fatfs_file_read:
	push	b
	push	dpl		; preserve handle pointer for caller
	push	dph
	push	0
	push	1
	push	2
	push	3
	push	4
	push	5
	push	6
	push	7

	mov	fatfs_file_scratch_file,dpl
	mov	fatfs_file_scratch_file+1,dph

	; Save dst (in dptr') to iram — ensure_block's new contract
	; overwrites dptr' with the cached pointer, so we need to put
	; dst back in dptr_copy's active slot manually.
	inc	auxr1
	mov	fatfs_file_scratch_dst,dpl
	mov	fatfs_file_scratch_dst+1,dph
	inc	auxr1

	mov	b,a		; b = n (uint8)

	; load offset into r4-r7 (via r0-r3 + xch)
	mov	dpl,fatfs_file_scratch_file
	mov	dph,fatfs_file_scratch_file+1
	mov	a,#fatfs_file_offset_off
	lcall	dptr_index
	lcall	dptr_read_32
	lcall	xch_r0123_r4567		; r4-r7 = offset

	; load size into r0-r3
	mov	dpl,fatfs_file_scratch_file
	mov	dph,fatfs_file_scratch_file+1
	mov	a,#fatfs_file_size_off
	lcall	dptr_index
	lcall	dptr_read_32		; r0-r3 = size

	; remaining = size - offset
	lcall	sub_32

	; if remaining == 0, return 0
	mov	a,r0
	orl	a,r1
	orl	a,r2
	orl	a,r3
	jnz	fatfs_file_read__has_rem
	clr	a
	ljmp	fatfs_file_read__return

fatfs_file_read__has_rem:
	; cap remaining at n: if any high byte nonzero, remaining >= 256
	; > n (n is uint8), so the cap is n. Otherwise compare r0 vs n.
	mov	a,r1
	orl	a,r2
	orl	a,r3
	jz	fatfs_file_read__rem_low
	; remaining > 255 → cap = n
	mov	a,b
	sjmp	fatfs_file_read__cap_done
fatfs_file_read__rem_low:
	mov	a,r0
	clr	c
	subb	a,b
	jc	fatfs_file_read__cap_r0
	mov	a,b
	sjmp	fatfs_file_read__cap_done
fatfs_file_read__cap_r0:
	mov	a,r0
fatfs_file_read__cap_done:
	mov	b,a			; b = bytes_to_copy (uint8)
	jnz	fatfs_file_read__nonzero
	clr	a
	ljmp	fatfs_file_read__return

fatfs_file_read__nonzero:
	; reload offset into r0-r3 (lost in sub_32)
	mov	dpl,fatfs_file_scratch_file
	mov	dph,fatfs_file_scratch_file+1
	mov	a,#fatfs_file_offset_off
	lcall	dptr_index
	lcall	dptr_read_32		; r0-r3 = offset

	; cache offset_in_block (low 9 bits = r0 + (r1 & 1) << 8) for
	; later dptr adjustment.
	push	0			; offset_in_block low byte
	mov	a,r1
	anl	a,#0x01
	push	acc			; offset_in_block high bit (0 or 1)

	; block_in_chain = offset >> 9
	mov	a,#9
	lcall	shr_32			; r0-r3 = block_in_chain

	; ensure the block is loaded. New contract: dptr (handle) is
	; preserved; dptr' returns the borrowed cached pointer.
	mov	dpl,fatfs_file_scratch_file
	mov	dph,fatfs_file_scratch_file+1
	lcall	fatfs_chain_ensure_block

	; Advance dptr' (cached) by offset_in_block.
	inc	auxr1			; switch to dptr' = cached
	pop	acc			; high bit
	mov	r4,a
	pop	acc			; low byte
	add	a,dpl
	mov	dpl,a
	mov	a,r4
	addc	a,dph
	mov	dph,a
	; dptr' (active) = cached + offset_in_block

	; For dptr_copy we need active = dst, inactive = src. Switch
	; back to the handle slot and load dst from scratch (dst was
	; clobbered when ensure_block wrote cached into the dptr' slot).
	inc	auxr1
	mov	dpl,fatfs_file_scratch_dst
	mov	dph,fatfs_file_scratch_dst+1

	mov	a,b
	mov	r0,a
	lcall	dptr_copy
	; slot 0 (active) = dst + b, slot 1 = cached + offset_in_block + b

	; Restore dptr' (slot 1) to the caller's original dst — promise
	; to preserve dptr' per the calling convention.
	inc	auxr1
	mov	dpl,fatfs_file_scratch_dst
	mov	dph,fatfs_file_scratch_dst+1
	inc	auxr1

	; offset += b (32-bit add of (b, 0, 0, 0))
	mov	dpl,fatfs_file_scratch_file
	mov	dph,fatfs_file_scratch_file+1
	mov	a,#fatfs_file_offset_off
	lcall	dptr_index
	push	dpl
	push	dph
	lcall	dptr_read_32		; r0-r3 = old offset
	mov	a,b
	mov	r4,a
	mov	r5,#0
	mov	r6,#0
	mov	r7,#0
	lcall	add_32
	pop	dph
	pop	dpl
	lcall	dptr_write_32

	; return value = b
	mov	a,b

fatfs_file_read__return:
	; a = return value; preserve through the pops
	mov	b,a		; stash in b (caller's b is already on stack)
	pop	7
	pop	6
	pop	5
	pop	4
	pop	3
	pop	2
	pop	1
	pop	0
	pop	dph
	pop	dpl
	mov	a,b
	pop	b
	ret
