
; *** fatfs/dir_iter.inc ***

; Directory iterator. Walks the 32-byte directory entries of a
; directory whose data is stored in a cluster chain.
;
; The dir handle is an *extension* of the chain handle from chain.inc:
; the chain header sits at offset 0 and is initialized via
; fatfs_chain_init. dir_open allocates the whole extended struct in
; one shot; dir_close releases it the same way. We never allocate or
; free the chain prefix separately.
;
; Layout (45 bytes; memory_allocate rounds to 48):
;   bytes 0..7   chain handle (first cluster, current cluster)
;   bytes 8..11  relative block number within the chain (uint32)
;   byte  12     entry index within the current block (0..16)
;                  (16 = "consumed the whole block, advance on next call")
;   bytes 13..44 embedded 32-byte directory entry buffer
;
; The embedded buffer is what dir_next returns to callers — we can't
; hand back a pointer into the block cache because we don't own its
; eviction. The buffer's lifetime is until the next dir_next or
; dir_close on the same handle.

.equ	fatfs_dir_block,	fatfs_chain_size	; ..+3
.equ	fatfs_dir_entry,	fatfs_dir_block+4
.equ	fatfs_dir_buffer,	fatfs_dir_entry+1	; ..+31
.equ	fatfs_dir_size,		fatfs_dir_buffer+32


; >> char *{dptr} fatfs_dir_open(uint32{r0-r3} start_cluster) <<

; Allocate a dir handle on the heap, initialize the embedded chain
; handle to start_cluster, set the position to (block 0, entry 0).
; Returns a pointer to the handle in dptr.

fatfs_dir_open:
	push	acc
	push	0
	push	1

	mov	r0,#fatfs_dir_size
	mov	r1,#0
	lcall	memory_allocate	; dptr = handle, r0-r7 preserved

	pop	1
	pop	0

	; init chain prefix with start_cluster (r0-r3 still hold it)
	lcall	fatfs_chain_init	; preserves dptr (still = handle)

	; zero out block_idx (handle[fatfs_dir_block..]) and entry_idx
	; (handle[fatfs_dir_entry]). acc was already pushed at the top
	; of the function; r0 needs its own save because it's a loop
	; counter here.
	push	0
	push	dpl
	push	dph

	mov	a,#fatfs_dir_block
	lcall	dptr_index		; dptr = handle + fatfs_dir_block
	clr	a
	mov	r0,#fatfs_dir_buffer-fatfs_dir_block
fatfs_dir_open__zero_loop:
	movx	@dptr,a
	inc	dptr
	djnz	r0,fatfs_dir_open__zero_loop

	pop	dph
	pop	dpl
	pop	0
	pop	acc
	ret


; >> void fatfs_dir_close(char *{dptr} handle) <<

; Deinitialize the chain prefix and free the whole dir handle.

fatfs_dir_close:
	push	acc
	push	0
	push	1

	lcall	fatfs_chain_deinit	; no-op currently, but symmetric
	mov	r0,#fatfs_dir_size
	mov	r1,#0
	lcall	memory_release

	pop	1
	pop	0
	pop	acc
	ret


; >> char *{dptr'} fatfs_dir_next(char *{dptr} handle) → status{a} <<

; Fetch the next directory entry into the handle's embedded buffer.
; On status == 0 the embedded buffer holds a valid 32-byte short-name
; entry and dptr' points at its first byte. The pointer is valid
; until the next dir_next / dir_close on this handle.
;
; Status codes:
;   0 - entry valid
;   1 - end of directory (first byte 0x00)
;
; Skips deleted entries (first byte 0xE5) and LFN entries (attribute
; byte == 0x0F) internally — the caller only sees short-name entries.
;
; dptr (handle) is preserved.

fatfs_dir_next:
	push	b
	push	0
	push	1
	push	2
	push	3
	push	4
	push	5
	push	6
	push	7
	push	dpl		; save handle (restored at end)
	push	dph

	; Keep the handle pointer in r6r7 across the function body so we
	; can reset dptr to it whenever a callee clobbers dptr.
	mov	r6,dpl
	mov	r7,dph

	; load entry_idx into r4
	mov	a,#fatfs_dir_entry
	lcall	dptr_index		; dptr = handle + 12
	movx	a,@dptr
	mov	r4,a			; r4 = entry_idx (sticky across loop)

fatfs_dir_next__loop:
	; if entry_idx == 16: reset to 0, increment block_idx
	cjne	r4,#16,fatfs_dir_next__within_block
	mov	r4,#0

	; load block_idx into r0-r3, increment, store back
	mov	dpl,r6
	mov	dph,r7
	mov	a,#fatfs_dir_block
	lcall	dptr_index		; dptr = handle + 8
	lcall	dptr_read_32		; r0-r3 = block_idx, dptr += 4

	inc	r0
	cjne	r0,#0,fatfs_dir_next__inc_done
	inc	r1
	cjne	r1,#0,fatfs_dir_next__inc_done
	inc	r2
	cjne	r2,#0,fatfs_dir_next__inc_done
	inc	r3
fatfs_dir_next__inc_done:

	mov	dpl,r6
	mov	dph,r7
	mov	a,#fatfs_dir_block
	lcall	dptr_index
	lcall	dptr_write_32		; handle[8..11] = block_idx
	sjmp	fatfs_dir_next__after_block

fatfs_dir_next__within_block:
	; reload block_idx into r0-r3 (the within-block branch — block
	; was not just modified, but ensure_block needs it in r0-r3)
	mov	dpl,r6
	mov	dph,r7
	mov	a,#fatfs_dir_block
	lcall	dptr_index
	lcall	dptr_read_32
fatfs_dir_next__after_block:

	; ensure_block(handle, block_idx). dptr (handle) is preserved;
	; dptr' returns the cached block start.
	mov	dpl,r6
	mov	dph,r7
	lcall	fatfs_chain_ensure_block

	; Advance dptr' (cached) by entry_idx * 32 to point at the
	; current entry. entry_idx is 0..15, so entry_idx * 32 fits in 9
	; bits — shift left 5, fold the carry-out (bit 8) into dph,
	; let dptr_index handle the dpl→dph carry of the low byte.
	inc	auxr1			; switch to dptr' = cached
	mov	a,r4
	swap	a		; * 16
	clr	c
	rlc	a		; * 32; c = bit 8, a = low byte
	jnc	fatfs_dir_next__no_high_bit
	inc	dph
fatfs_dir_next__no_high_bit:
	lcall	dptr_index	; dpl += a, with carry into dph
	; dptr' now points at the entry inside the block cache.

	; Copy 32 bytes from cached (= dptr') to the handle's embedded
	; buffer. dptr_copy uses dptr = dst, dptr' = src — so we switch
	; back to dptr (handle), advance to handle + fatfs_dir_buffer,
	; and dptr_copy reads from inactive (= cached + entry offset).
	inc	auxr1			; active = dptr = handle
	mov	a,#fatfs_dir_buffer
	lcall	dptr_index	; active = handle + 13

	mov	r0,#32
	lcall	dptr_copy	; copies 32 bytes; both pointers advance by 32

	; classify the entry — load first byte from the buffer. After
	; dptr_copy, active = handle + 13 + 32. Reset to the buffer
	; start.
	mov	dpl,r6
	mov	dph,r7
	mov	a,#fatfs_dir_buffer
	lcall	dptr_index	; dptr = handle + 13
	movx	a,@dptr		; first byte

	jnz	fatfs_dir_next__not_eod
	; first byte == 0x00 → end of directory. Don't advance entry_idx.
	mov	r5,#1		; status = 1
	sjmp	fatfs_dir_next__persist

fatfs_dir_next__not_eod:
	cjne	a,#0xE5,fatfs_dir_next__not_deleted
	; deleted entry: advance, loop
	inc	r4
	ljmp	fatfs_dir_next__loop

fatfs_dir_next__not_deleted:
	; check attribute byte at offset 11 of the entry buffer
	mov	dpl,r6
	mov	dph,r7
	mov	a,#fatfs_dir_buffer+11
	lcall	dptr_index
	movx	a,@dptr
	cjne	a,#0x0F,fatfs_dir_next__valid
	; LFN: advance, loop
	inc	r4
	ljmp	fatfs_dir_next__loop

fatfs_dir_next__valid:
	; valid short-name entry: consume it and return status 0
	inc	r4
	mov	r5,#0

fatfs_dir_next__persist:
	; persist entry_idx (r4) into handle[12]
	mov	dpl,r6
	mov	dph,r7
	mov	a,#fatfs_dir_entry
	lcall	dptr_index
	mov	a,r4
	movx	@dptr,a

	; set dptr' = handle + 13 (entry buffer) for the caller
	inc	auxr1
	mov	dpl,r6
	mov	dph,r7
	mov	a,#fatfs_dir_buffer
	lcall	dptr_index
	inc	auxr1		; back to main dptr

	; restore handle pointer (caller's dptr) and registers. Move the
	; status into acc before popping r5 (which restores the caller's
	; value).
	pop	dph
	pop	dpl
	pop	7
	pop	6
	mov	a,r5		; status into return register
	pop	5
	pop	4
	pop	3
	pop	2
	pop	1
	pop	0
	pop	b
	ret
