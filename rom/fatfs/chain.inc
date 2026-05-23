
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


; >> char *{dptr} fatfs_chain_ensure_block(char *{dptr} handle, uint32{r0-r3} block) <<

; Make sure the chain's `block`-th 512-byte block is loaded in the
; block cache, and return a borrowed dptr into the cached data.
;
; Borrowed: valid only until the next block_load (e.g. another
; ensure_block call on any chain). Caller must copy out what it needs
; and not retain the pointer.
;
; Step 1 stub: only block == 0 is supported. Real traversal lands when
; the first caller needs to follow a chain past its first cluster, at
; which point fatfs_chain_next_cluster joins this file too.

fatfs_chain_ensure_block:
	push	acc
	push	0			; preserve r0-r3 (input, not output)
	push	1
	push	2
	push	3

	; verify block == 0 (stub limitation)
	mov	a,r0
	orl	a,r1
	orl	a,r2
	orl	a,r3
	jz	fatfs_chain_ensure_block__zero
	lcall	panic	; chain traversal not implemented yet
fatfs_chain_ensure_block__zero:

	; read first cluster number from handle[first] into r0-r3.
	; dptr now points past the field; we don't need it after this.
	lcall	dptr_read_32

	; convert cluster number to first sector LBA
	lcall	fatfs_cluster_to_lba

	; load that block; dptr returns pointing at cached data
	lcall	block_load

	pop	3
	pop	2
	pop	1
	pop	0
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

	pop	0
	; active slot currently holds dst+512; restore it.
	pop	dph
	pop	dpl
	inc	auxr1
	; alternate slot currently holds cached+512; restore it
	; (back to the original handle pointer).
	pop	dph
	pop	dpl
	ret
