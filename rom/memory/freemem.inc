
; *** memory/getmem.inc ***

; >> void freemem(char *addr, uint32 size) <<

; Allocate memory, return lowest address

freemem:
	push	ie
	clr	ea
	
	pop	ie
	ret
