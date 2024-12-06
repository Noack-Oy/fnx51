
; *** memory/getmem.inc ***

; >> char *getmem(uint32 size) <<

; Allocate memory, return lowest address

getmem:
	push	ie
	clr	ea
	
	pop	ie
	ret
