
; *** fatfs/init.inc ***

; >> void fatfs_init() <<

; Initialize FAT file system.
; Search for a valid FAT file system on the current block device.
; Memory is allocated to store the necessary data to handle the file
; system, and a pointer to that is put into the fatfs_info variable in 
; internal RAM. Currently only FAT32 on MBR partition 0 is supported.

fatfs_init:
	push	acc
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

	; partition table begins at offset:	446 = 0x1be
	; the type code is at that +4:		450 = 0x1bc
	mov	a,dpl
	add	a,#0xbc
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
	xor	a,#0x02
	jz	fatfs_init__bps_ok_2
	acall	panic	; invalid bytes per sector
fatfs_init__bps_ok2:
	inc	dptr

	; read sectors per cluster (offset 0x0d) and validate
	movx	a,@dptr
	jnz	fatfs_init__spc_ok_1
	acall	panic	; zero sectors per cluster not allowed
fatfs_init__spc_ok_1:
	mov	r0,a
	dec	a
	anl	a,r0	; power of two check: (x & (x-1)) == 0
	jz	fatfs_init__spc_ok_2
	acall	panic	; sectors per cluster must be power of 2
fatfs_init_spc_ok_2:
	mov	a,r0	; valid value now in a
	; ... and store into fatfs_info data structure at offset 4
	inc	auxr1	; switch to other dptr
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	inc	dptr
	inc	dptr
	inc	dptr
	inc	dptr
	mov	@dptr,a
	inc	auxr1	; restore original dptr
	inc	dptr

	; read number of reserved sectors at offset 0x0e
	mov	a,@dptr
	mov	r4,a
	inc	dptr
	mov	a,@dptr
	mov	r5,a
	clr	a
	mov	r6,a
	mov	r6,a
	inc	dptr
	; now calculate fat1_lba = part_lba + reserved_sectors
	inc	auxr1
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	acall	dptr_read_32 ; also moves dptr forward to offset 4
	acall	add_32
	; and store into fatfs_info data structure at offset 5
	inc	dptr
	acall	dptr_write_32
	inc	auxr1

	; read number of FATs at offset 0x10, must always be 2

	acall	regbank_prev
	pop	dph
	pop	dpl
	inc	auxr1
	pop	dph
	pop	dpl
	pop	acc
	ret
