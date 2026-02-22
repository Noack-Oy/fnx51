
; *** fatfs/init.inc ***

; >> void fatfs_init() <<

; Initialize FAT file system.
; Search for a valid FAT file system on the current block device.
; Memory is allocated to store the necessary data to handle the file
; system, and a pointer to that is put into the fatfs_info variable in 
; internal RAM. Currently only FAT32 on MBR partition 0 is supported.

fatfs_init:
	push	acc
	push	b
	push	dpl
	push	dph
	inc	auxr1
	push	dpl
	push	dph
	acall	regbank_next

	mov	r0,fatfs_info_size
	mov	r1,#0
	acall	memory_allocate
	mov	fatfs_info,r0
	mov	fatfs_info+1,r1

	; read sector 0 (MBR)
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	acall	block_load

	acall	fatfs_init__check_signature

	; partition table begins at offset:	446 = 0x1be
	; the type code is at that +4:		450 = 0x1c2
	mov	a,dpl
	add	a,#0xc2
	mov	dpl,a
	mov	a,dph
	addc	a,#0x01
	mov	dph,a
	movx	a,@dptr	; read type code
	clr	c
	subb	a,#0x0b
	jz	fatfs_init__type_code_ok
	dec	a ; also accept 0x0c
	jz	fatfs_init__type_code_ok
	acall	panic ; invalid type code
fatfs_init__type_code_ok:

	; read partition start LBA at another +4 offset from type code
	inc	dptr
	inc	dptr
	inc	dptr
	inc	dptr
	inc	auxr1
	; ... and store into fatfs_info (at offset 0)
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	mov	r0,#4
	acall	dptr_copy

	; read FAT32 volume ID (first sector in partition)
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	acall	dptr_read_32
	acall	block_load

	acall	fatfs_init__check_signature

	; validate bytes per sector (offset 0x0b), must be 512
	mov	a,dpl
	add	a,#0x0b
	mov	dpl,a
	mov	a,dph
	addc	a,#0x00
	mov	dph,a
	movx	a,@dptr ; low byte: must be 0x00
	jz	fatfs_init__bps_ok_1
	acall	panic	; invalid bytes per sector
fatfs_init__bps_ok_1:
	inc	dptr
	movx	a,@dptr ; high byte: must be 0x02
	xrl	a,#0x02
	jz	fatfs_init__bps_ok_2
	acall	panic	; invalid bytes per sector
fatfs_init__bps_ok_2:
	inc	dptr

	; read sectors per cluster (offset 0x0d) and validate
	movx	a,@dptr
	jnz	fatfs_init__spc_ok_1
	acall	panic	; zero sectors per cluster not allowed
fatfs_init__spc_ok_1:
	mov	b,a
	dec	a
	anl	a,b	; power of two check: (x & (x-1)) == 0
	jz	fatfs_init__spc_ok_2
	acall	panic	; sectors per cluster must be power of 2
fatfs_init__spc_ok_2:
	mov	a,b	; value in b is also used later below!
	; ... and store into fatfs_info data structure at offset 4
	inc	auxr1	; switch to other dptr
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	inc	dptr
	inc	dptr
	inc	dptr
	inc	dptr
	movx	@dptr,a
	inc	auxr1	; restore original dptr
	inc	dptr

	; read number of reserved sectors at offset 0x0e
	movx	a,@dptr
	mov	r4,a
	inc	dptr
	movx	a,@dptr
	mov	r5,a
	clr	a
	mov	r6,a
	mov	r7,a
	inc	dptr
	; now calculate fat1_lba = part_lba + reserved_sectors
	inc	auxr1
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	acall	dptr_read_32 ; also moves dptr forward to offset 4
	acall	add_32
	; and store into fatfs_info data structure at offset 5
	inc	dptr
	acall	dptr_write_32 ; advances to offset 9
	inc	auxr1 ; swich back to block read pointer

	; validate number of FATs at offset 0x10, must always be 2
	movx	a,@dptr
	xrl	a,#0x02
	jz	fatfs_init__nof_ok
	acall	panic	; invalid number of FATs
fatfs_init__nof_ok:

	; skip forward to offset 0x24 (+0x14)
	mov	a,dpl
	add	a,#0x14
	mov	dpl,a
	mov	a,dph
	addc	a,#0x00
	mov	dph,a

	; r0-r3 still has fat1_lba here from above, shunt into r4-r7
	acall	xch_r0123_r4567

	; read sectors per fat at offset 0x24
	acall	dptr_read_32	; advances to offset 0x28
	; now calculate fat2_lba by adding sectors per FAT to fat1_lba
	acall	xch_r0123_r4567
	acall	add_32
	; and store into fatfs_info data structure at offset 9
	inc	auxr1 ; swich to fatfs_info write pointer
	acall	dptr_write_32	; advances to offset 13
	; now if we add sectors per FAT again, get cluster_lba
	acall	add_32
	; however, that is the address of cluster 2,
	; we wind back to the (nonexistent) cluster 0,
	; this makes math easier later
	mov	r4,b	; b should still have sectors per cluster here
	clr	a
	mov	r5,a
	mov	r6,a
	mov	r7,a
	acall	sub_32
	acall	sub_32
	; now we can store cluster_lba at the current offset (13)
	acall	dptr_write_32 ; advances to offset 17
	inc	auxr1	; back to the block reading pointer

	; move forward to offset 0x2c
	inc	dptr
	inc	dptr
	inc	dptr
	inc	dptr
	; now read the cluster number for the root directory
	acall	dptr_read_32
	; ... and store into fatfs_info at current offset (17)
	inc	auxr1
	acall	dptr_write_32

	; phew! that was a lot of shunting data around. we are done now.

	acall	regbank_prev
	pop	dph
	pop	dpl
	inc	auxr1
	pop	dph
	pop	dpl
	pop	b
	pop	acc
	ret

fatfs_init__check_signature:
	mov	a,dpl
	add	a,#0xfe
	mov	dpl,a
	mov	a,dph
	addc	a,#0x01
	mov	dph,a
	movx	a,@dptr ; read fist signature byte at offset 510
	xrl	a,#0x55
	jz	fatfs_init__check_signature_ok_1
	acall	panic ; invalid signature
fatfs_init__check_signature_ok_1:
	inc	dptr
	movx	a,@dptr ; read second signature byte at offset 511
	xrl	a,#0xaa
	jz	fatfs_init__check_signature_ok_2
	acall	panic ; invalid signature
fatfs_init__check_signature_ok_2:
	inc	dptr ; this moves dptr to offset 512
	dec	dph  ; so we can now wind back to the beginning
	dec	dph  ; by decrementing dph twice
	ret
