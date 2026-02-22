
; *** fatfs/cluster.inc ***

; >> uint32{r0-r3} fatfs_cluster_to_lba(uint32{r0-r3} cluster) <<

; Convert cluster number to first sector LBA.
; Uses the formula: LBA = cluster_lba + (cluster * cluster_size)
; Note: cluster_lba is already adjusted for theoretical cluster #0,
; so no subtraction of 2 is needed.

fatfs_cluster_to_lba:
	push	acc
	push	b
	push	dpl
	push	dph
	mov	a,r4
	push	acc
	mov	a,r5
	push	acc
	mov	a,r6
	push	acc
	mov	a,r7
	push	acc

	; Load cluster_size from fatfs_info+4
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	mov	a,#fatfs_info_cluster_size
	acall	dptr_index
	movx	a,@dptr
	mov	b,a			; b = cluster_size

	; Calculate log2 of cluster_size by counting right shifts until we get 1
	; cluster_size must be a power of 2 (1, 2, 4, 8, 16, 32, 64, 128)
	mov	a,b			; a = cluster_size
	mov	r4,#0			; shift count starts at 0
fatfs_cluster_to_lba__count_loop:
	cjne	a,#0x01,fatfs_cluster_to_lba__not_one
	sjmp	fatfs_cluster_to_lba__shifts_counted
fatfs_cluster_to_lba__not_one:
	jz	fatfs_cluster_to_lba__bad_size	; zero is invalid
	clr	c
	rrc	a			; right shift by 1
	jc	fatfs_cluster_to_lba__bad_size	; not a power of 2 if carry set
	inc	r4			; increment shift count
	sjmp	fatfs_cluster_to_lba__count_loop
fatfs_cluster_to_lba__bad_size:
	; Invalid cluster size (not a power of 2)
	acall	panic
	
fatfs_cluster_to_lba__shifts_counted:
	; Now r4 has the shift count
	; Shift r0-r3 left by r4 positions (cluster * cluster_size)
	mov	a,r4
	acall	shl_32

	; r0-r3 now contains cluster * cluster_size
	; Now we need to add cluster_lba from fatfs_info+13
	; Move shifted cluster to r4-r7
	acall	xch_r0123_r4567

	; Load cluster_lba into r0-r3
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	mov	a,#fatfs_info_cluster_lba
	acall	dptr_index
	acall	dptr_read_32		; r0-r3 = cluster_lba

	; Add cluster offset: r0-r3 += r4-r7
	acall	add_32

	pop	acc
	mov	r7,a
	pop	acc
	mov	r6,a
	pop	acc
	mov	r5,a
	pop	acc
	mov	r4,a
	pop	dph
	pop	dpl
	pop	b
	pop	acc
	ret
