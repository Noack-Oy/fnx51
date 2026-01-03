
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
	mov	a,#0xbc
	add	a,dpl
	mov	dpl,a
	mov	a,#0x1
	addc	a,dph
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
	; .., and store into fatfs_info (at offset 0)
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	mov	r0,#4
	acall	dptr_copy

	; read FAT32 volume ID (first sector in partition)
	mov	dpl,fatfs_info
	mov	dph,fatfs_info+1
	acall	dptr_read_32
	acall	block_load

	acall	panic ; TODO: continue here

	acall	regbank_prev
	pop	dph
	pop	dpl
	inc	auxr1
	pop	dph
	pop	dpl
	pop	acc
	ret
