
; *** fatfs/chain.inc ***

; A "chain handle" abstracts cluster-chain traversal. Directories and
; files both look like a linear sequence of 512-byte blocks; the handle
; owns the state needed to map a relative block number to an LBA.
;
; See chain.equ for the layout (first cluster, current cluster,
; current cluster's chain index).
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

; Internal scratch — ensure_block needs a stable place for the handle
; pointer that survives popping target_idx into r4-r7. Two iram bytes;
; only valid during a single ensure_block call (not re-entrant).

.equ	fatfs_chain_scratch_handle,	0x52	; ..0x53


; >> void fatfs_chain_init(char *{dptr} handle, uint32{r0-r3} first_cluster) <<

; Initialize a pre-allocated chain handle at dptr:
;   first cluster   <- r0-r3
;   current cluster <- r0-r3 (current starts at first)
;   current idx     <- 0
; Preserves dptr and r0-r3.

fatfs_chain_init:
	push	dpl
	push	dph
	push	0
	push	1
	push	2
	push	3

	lcall	dptr_write_32	; handle[first] = first_cluster
	lcall	dptr_write_32	; handle[current] = first_cluster

	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	lcall	dptr_write_32	; handle[8..11] = 0 (current index)

	pop	3
	pop	2
	pop	1
	pop	0
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


; >> uint32{r0-r3} fatfs_chain_next_cluster(uint32{r0-r3} cluster) <<

; Look up the next-cluster entry in the FAT for the given cluster.
; Each FAT sector holds 128 four-byte entries.
;   LBA              = fat1_lba + (cluster / 128)
;   offset_in_sector = (cluster % 128) * 4
; Returns the next cluster number; values >= 0x0FFFFFF8 mean
; end-of-chain. The high nibble of the 32-bit entry is reserved by
; FAT32 and is masked off.

fatfs_chain_next_cluster:
	push	acc
	push	b
	push	dpl
	push	dph
	push	4
	push	5
	push	6
	push	7

	; b = cluster mod 128 (low 7 bits of cluster low byte)
	mov	a,r0
	anl	a,#0x7F
	mov	b,a

	; r0-r3 = cluster >> 7 (sector offset within the FAT)
	mov	a,#7
	lcall	shr_32

	; shunt offset into r4-r7
	lcall	xch_r0123_r4567

	; r0-r3 = fat1_lba
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	mov	a,#fatfs_info_fat1_lba
	lcall	dptr_index
	lcall	dptr_read_32

	; r0-r3 = fat1_lba + sector_offset
	lcall	add_32

	; load the FAT sector
	lcall	block_load		; dptr = cached FAT sector

	; advance dptr by (entry_idx_in_sector * 4). entry_idx is 0..127,
	; so *4 is 0..508 — fits in 9 bits.
	mov	a,b
	clr	c
	rlc	a		; * 2
	clr	c
	rlc	a		; * 4; c holds the 9th bit
	mov	r4,a		; r4 = low byte
	clr	a
	rlc	a		; a = high bit
	mov	r5,a		; r5 = high byte

	mov	a,dpl
	add	a,r4
	mov	dpl,a
	mov	a,dph
	addc	a,r5
	mov	dph,a

	; read the four bytes of the FAT entry
	lcall	dptr_read_32

	; mask off the high nibble of r3 (FAT32 reserved bits).
	mov	a,r3
	anl	a,#0x0F
	mov	r3,a

	pop	7
	pop	6
	pop	5
	pop	4
	pop	dph
	pop	dpl
	pop	b
	pop	acc
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
; Algorithm:
;   target_idx       = block / cluster_size
;   sector_in_cluster = block % cluster_size
;   adjust the handle's (current_cluster, current_idx) until
;     current_idx == target_idx:
;       - if current_idx > target_idx: restart from first cluster
;       - if current_idx < target_idx: step forward via next_cluster
;   LBA = cluster_to_lba(current_cluster) + sector_in_cluster
;   block_load(LBA)

fatfs_chain_ensure_block:
	push	acc
	push	b			; ensure_block uses b for cluster_size
	push	0			; preserve r0-r3 (input, not output)
	push	1
	push	2
	push	3
	push	4
	push	5
	push	6
	push	7

	; stash handle pointer in iram (r6r7 gets reused for target_idx
	; pops later, so we can't keep it there)
	mov	fatfs_chain_scratch_handle,dpl
	mov	fatfs_chain_scratch_handle+1,dph

	; load cluster_size into b (dptr is temporarily clobbered). Save
	; r0 since we want block.low byte later for sector_in_cluster.
	push	0
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	mov	a,#fatfs_info_cluster_size
	lcall	dptr_index
	movx	a,@dptr
	mov	b,a		; b = cluster_size
	pop	0

	; sector_in_cluster = block.low & (cluster_size - 1)
	mov	a,b
	dec	a		; mask = cluster_size - 1
	anl	a,r0
	push	acc		; stack: ..., sector_in_cluster

	; log2(cluster_size) → r4 (count of right shifts until == 1)
	mov	a,b
	mov	r4,#0
fatfs_chain_ensure_block__log2_loop:
	cjne	a,#1,fatfs_chain_ensure_block__log2_not_one
	sjmp	fatfs_chain_ensure_block__log2_done
fatfs_chain_ensure_block__log2_not_one:
	clr	c
	rrc	a
	inc	r4
	sjmp	fatfs_chain_ensure_block__log2_loop
fatfs_chain_ensure_block__log2_done:

	; target_idx = block >> log2(cluster_size)
	mov	a,r4
	lcall	shr_32		; r0-r3 = target_idx

	; stash target_idx on the stack across the walk loop
	push	0
	push	1
	push	2
	push	3		; stack: ..., sector_in_cluster, t0, t1, t2, t3

fatfs_chain_ensure_block__walk_loop:
	; load handle.current_cluster_idx into r0-r3
	mov	dpl,fatfs_chain_scratch_handle
	mov	dph,fatfs_chain_scratch_handle+1
	mov	a,#8
	lcall	dptr_index
	lcall	dptr_read_32	; r0-r3 = current_idx

	; pop target_idx into r4-r7 for the comparison
	pop	7
	pop	6
	pop	5
	pop	4
	; stack: ..., sector_in_cluster

	; r0-r3 (current) vs r4-r7 (target) — equality check via XOR
	mov	a,r0
	xrl	a,r4
	jnz	fatfs_chain_ensure_block__not_equal
	mov	a,r1
	xrl	a,r5
	jnz	fatfs_chain_ensure_block__not_equal
	mov	a,r2
	xrl	a,r6
	jnz	fatfs_chain_ensure_block__not_equal
	mov	a,r3
	xrl	a,r7
	jnz	fatfs_chain_ensure_block__not_equal
	ljmp	fatfs_chain_ensure_block__walk_done

fatfs_chain_ensure_block__not_equal:
	; ordering check: high byte down. carry set after subb means
	; current_byte < target_byte → current < target → step forward.
	mov	a,r3
	clr	c
	subb	a,r7
	jc	fatfs_chain_ensure_block__step_forward
	jnz	fatfs_chain_ensure_block__restart
	mov	a,r2
	clr	c
	subb	a,r6
	jc	fatfs_chain_ensure_block__step_forward
	jnz	fatfs_chain_ensure_block__restart
	mov	a,r1
	clr	c
	subb	a,r5
	jc	fatfs_chain_ensure_block__step_forward
	jnz	fatfs_chain_ensure_block__restart
	mov	a,r0
	clr	c
	subb	a,r4
	jc	fatfs_chain_ensure_block__step_forward
	; jnz reachable here only if current > target
	; (equality is handled above)

fatfs_chain_ensure_block__restart:
	; current > target → reset current to first cluster.
	; push target back for next iteration.
	push	4
	push	5
	push	6
	push	7

	; read first_cluster (handle[0..3]) into r0-r3
	mov	dpl,fatfs_chain_scratch_handle
	mov	dph,fatfs_chain_scratch_handle+1
	lcall	dptr_read_32	; dptr now handle+4

	; write to current_cluster (handle[4..7])
	lcall	dptr_write_32	; dptr now handle+8

	; write 0 to current_idx (handle[8..11])
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	lcall	dptr_write_32

	sjmp	fatfs_chain_ensure_block__walk_loop

fatfs_chain_ensure_block__step_forward:
	; current < target → advance by one cluster.
	push	4
	push	5
	push	6
	push	7

	; r0-r3 = handle.current_cluster
	mov	dpl,fatfs_chain_scratch_handle
	mov	dph,fatfs_chain_scratch_handle+1
	mov	a,#4
	lcall	dptr_index
	lcall	dptr_read_32

	; r0-r3 = next_cluster(current_cluster)
	lcall	fatfs_chain_next_cluster

	; store back to handle.current_cluster
	mov	dpl,fatfs_chain_scratch_handle
	mov	dph,fatfs_chain_scratch_handle+1
	mov	a,#4
	lcall	dptr_index
	lcall	dptr_write_32

	; current_idx++
	mov	dpl,fatfs_chain_scratch_handle
	mov	dph,fatfs_chain_scratch_handle+1
	mov	a,#8
	lcall	dptr_index
	lcall	dptr_read_32	; r0-r3 = current_idx, dptr now handle+12

	inc	r0
	cjne	r0,#0,fatfs_chain_ensure_block__inc_done
	inc	r1
	cjne	r1,#0,fatfs_chain_ensure_block__inc_done
	inc	r2
	cjne	r2,#0,fatfs_chain_ensure_block__inc_done
	inc	r3
fatfs_chain_ensure_block__inc_done:

	mov	dpl,fatfs_chain_scratch_handle
	mov	dph,fatfs_chain_scratch_handle+1
	mov	a,#8
	lcall	dptr_index
	lcall	dptr_write_32

	ljmp	fatfs_chain_ensure_block__walk_loop

fatfs_chain_ensure_block__walk_done:
	; stack: ..., sector_in_cluster
	; r0-r3 = current_idx (= target_idx)

	; load current_cluster (handle[4..7]) into r0-r3
	mov	dpl,fatfs_chain_scratch_handle
	mov	dph,fatfs_chain_scratch_handle+1
	mov	a,#4
	lcall	dptr_index
	lcall	dptr_read_32

	; r0-r3 = cluster_to_lba(current_cluster)
	lcall	fatfs_cluster_to_lba

	; LBA += sector_in_cluster
	pop	acc		; a = sector_in_cluster
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

	; Load the block into the alternate dptr so the caller's dptr
	; (= handle) survives. ensure_block's new contract: dptr = handle
	; (preserved), dptr' = cached.
	inc	auxr1
	lcall	block_load	; dptr' (now active) = cached
	inc	auxr1

	; Restore dptr = handle from iram scratch.
	mov	dpl,fatfs_chain_scratch_handle
	mov	dph,fatfs_chain_scratch_handle+1

	pop	7
	pop	6
	pop	5
	pop	4
	pop	3
	pop	2
	pop	1
	pop	0
	pop	b
	pop	acc
	ret


; >> void fatfs_chain_load_block(char *{dptr} handle, uint32{r0-r3} block, char *{dptr'} dst) <<

; Convenience wrapper that loads a chain block into the caller's own
; 512-byte buffer (passed as dst in the alternate dptr). Built on
; fatfs_chain_ensure_block plus a 512-byte memcpy.
;
; May not see callers in practice — dir-iter and file-read both prefer
; ensure_block so they can copy only the bytes they actually need.

fatfs_chain_load_block:
	; save active slot (handle pointer)
	push	dpl
	push	dph
	inc	auxr1
	; save alternate slot (dst pointer)
	push	dpl
	push	dph
	inc	auxr1
	; r0 is the only general register we modify here (dptr_copy's
	; count). r1-r7 are preserved across ensure_block and dptr_copy.
	push	0

	; ensure_block uses the active dptr as handle, block in r0-r3;
	; on return, active dptr = cached block data. ensure_block now
	; preserves r0-r3.
	lcall	fatfs_chain_ensure_block

	; swap so that the active dptr is dst (where dptr_copy writes),
	; and the alternate dptr is the cached block (where dptr_copy
	; reads).
	inc	auxr1

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
