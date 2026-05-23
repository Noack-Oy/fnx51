
; *** fatfs/chain.inc ***

; A "chain handle" abstracts cluster-chain traversal. Directories and
; files both look like a linear sequence of 512-byte blocks; the handle
; owns the state needed to map a relative block number to an LBA.
;
; See chain.equ for the layout (first cluster, current cluster).
;
; Lifecycle convention (mirrored by dir and file handles, which extend
; the chain handle by adding fields after fatfs_chain_size):
;
;   *_init   — initialize a pre-allocated struct. No allocation.
;   *_deinit — release sub-resources. No deallocation. Pairs with _init.
;   *_open   — convenience: memory_allocate + _init. Standalone use.
;   *_close  — convenience: _deinit + memory_release. Standalone use.
;
; Extension types call only _init / _deinit on the chain prefix; they
; own the outer allocation themselves.


; >> void fatfs_chain_init(char *{dptr} handle, uint32{r0-r3} first_cluster) <<

; Initialize a pre-allocated chain handle at dptr with first and
; current cluster both set to first_cluster. Preserves dptr and r0-r3.

fatfs_chain_init:
	push	dpl
	push	dph

	lcall	dptr_write_32	; handle[first] = first_cluster
	lcall	dptr_write_32	; handle[current] = first_cluster

	pop	dph
	pop	dpl
	ret


; >> void fatfs_chain_deinit(char *{dptr} handle) <<

; Release sub-resources held by the chain handle. Currently a no-op;
; declared so extension types can call it unconditionally.

fatfs_chain_deinit:
	ret


; >> char *{dptr} fatfs_chain_open(uint32{r0-r3} first_cluster) <<

; Allocate a chain handle on the heap and initialize it with
; first_cluster. Returns a pointer to the handle in dptr.

fatfs_chain_open:
	push	0
	push	1

	mov	r0,#fatfs_chain_size
	mov	r1,#0
	lcall	memory_allocate	; dptr = handle, r0-r7 preserved

	pop	1
	pop	0
	; r0-r3 hold the caller's first_cluster again (memory_allocate
	; preserved r2-r3 across the call; r0r1 we restored above).
	lcall	fatfs_chain_init
	ret


; >> void fatfs_chain_close(char *{dptr} handle) <<

; Deinitialize a chain handle and free its allocation.

fatfs_chain_close:
	push	0
	push	1

	lcall	fatfs_chain_deinit
	mov	r0,#fatfs_chain_size
	mov	r1,#0
	lcall	memory_release

	pop	1
	pop	0
	ret


; >> char *{dptr'} fatfs_chain_ensure_block(char *{dptr} handle, uint32{r0-r3} block) <<

; Make sure the chain's `block`-th 512-byte block is loaded in the
; block cache, and return a borrowed dptr to the cached data in dptr'
; (the alternate data pointer). dptr (the handle) is preserved so
; callers don't have to stash it across the call.
;
; Borrowed: valid only until the next block_load (e.g. another
; ensure_block call on any chain). Caller must copy out what it needs
; and not retain the pointer.
;
; Step 2 stub: any block within the first cluster is supported
; (cluster_idx == 0, sector_in_cluster = block). Anything past the
; first cluster still panics — chain traversal and the matching
; fatfs_chain_next_cluster routine land when an actual caller needs
; to follow a chain that far.

fatfs_chain_ensure_block:
	push	acc
	push	0			; preserve r0-r3 (input, not output)
	push	1
	push	2
	push	3
	push	4
	push	5
	push	6
	push	7
	push	dpl			; preserve dptr (handle) for the caller
	push	dph

	; high three bytes of block must be zero (block fits in a byte;
	; cluster_size is at most 128).
	mov	a,r1
	orl	a,r2
	orl	a,r3
	jnz	fatfs_chain_ensure_block__panic

	; stash handle in r4r5 so we can use dptr to load cluster_size
	; and still get back to the handle for dptr_read_32.
	mov	r4,dpl
	mov	r5,dph

	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	mov	a,#fatfs_info_cluster_size
	lcall	dptr_index
	movx	a,@dptr			; a = cluster_size

	; block < cluster_size? (cluster_idx == 0 iff block < cluster_size)
	clr	c
	subb	a,r0			; a = cluster_size - r0
	jc	fatfs_chain_ensure_block__panic	; block > cluster_size
	jz	fatfs_chain_ensure_block__panic	; block == cluster_size

	; restore dptr = handle
	mov	dpl,r4
	mov	dph,r5

	; sector_in_cluster = r0. Park it in a — dptr_read_32 and
	; cluster_to_lba both push/pop acc, so it survives.
	mov	a,r0

	; read first_cluster from handle[fatfs_chain_first..] into r0-r3.
	lcall	dptr_read_32

	; convert to first sector LBA of that cluster
	lcall	fatfs_cluster_to_lba

	; LBA += sector_in_cluster (still in a)
	add	a,r0
	mov	r0,a
	clr	a
	addc	a,r1
	mov	r1,a
	clr	a
	addc	a,r2
	mov	r2,a
	clr	a
	addc	a,r3
	mov	r3,a

	; load the block into the alternate dptr so the caller's dptr
	; (= handle) survives.
	inc	auxr1
	lcall	block_load		; dptr' (now active) = cached
	inc	auxr1			; back to the caller's slot

	pop	dph
	pop	dpl
	pop	7
	pop	6
	pop	5
	pop	4
	pop	3
	pop	2
	pop	1
	pop	0
	pop	acc
	ret

fatfs_chain_ensure_block__panic:
	lcall	panic	; chain traversal not implemented yet


; >> void fatfs_chain_load_block(char *{dptr} handle, uint32{r0-r3} block, char *{dptr'} dst) <<

; Convenience wrapper that loads a chain block into the caller's own
; 512-byte buffer (passed as dst in the alternate dptr). Built on
; fatfs_chain_ensure_block plus a 512-byte memcpy.
;
; May not see callers in practice — dir-iter and file-read both prefer
; ensure_block so they can copy only the bytes they actually need.

fatfs_chain_load_block:
	push	0			; dptr_copy uses r0 as count

	; Save handle (slot 0). ensure_block preserves it, but we'll
	; temporarily overwrite slot 0 with dst to feed dptr_copy; need
	; the handle back at exit.
	push	dpl
	push	dph

	; Save dst (slot 1) — ensure_block will overwrite it with the
	; cached pointer.
	inc	auxr1
	push	dpl
	push	dph
	inc	auxr1

	; ensure_block leaves slot 0 = handle (preserved) and slot 1 =
	; cached (the borrowed pointer). dst is on the stack.
	lcall	fatfs_chain_ensure_block

	; Pop dst into slot 0 (overwriting handle for now). Stack top is
	; dst.dph; below: dst.dpl; below: handle.dph, handle.dpl.
	pop	dph
	pop	dpl
	; slot 0 = dst, slot 1 = cached.

	; dptr_copy with r0 = 0 copies 256 bytes (djnz wraps). Twice = 512.
	mov	r0,#0
	lcall	dptr_copy
	mov	r0,#0
	lcall	dptr_copy
	; slot 0 = dst+512, slot 1 = cached+512.

	; Restore dst (slot 0): two `dec dph` subtract 512 (dpl rolled
	; around an even number of times during the 512 inc dptrs).
	dec	dph
	dec	dph

	; Pop the saved handle into slot 1. The slots end up physically
	; swapped relative to entry (slot 0 = dst, slot 1 = handle), but
	; the calling convention only cares about `dptr` and `dptr'`
	; holding the right values, not which physical slot they live in.
	inc	auxr1
	pop	dph
	pop	dpl
	; active (slot 1) = handle, inactive (slot 0) = dst.

	pop	0
	ret
