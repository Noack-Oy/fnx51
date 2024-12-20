
; *** memory/release.inc ***

; >> void release(r0r1 *addr, r2r3 size) <<

; Allocate memory, return lowest address

memory_release:
	push	ie
	clr	ea
	
	pop	ie
	ret
