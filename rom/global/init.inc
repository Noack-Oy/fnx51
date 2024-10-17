
; *** global/init.inc ***

; Initialize stack pointer and global variables.
; This must be included early in the main program.

	mov	SP,	#STACK

	mov	dptr,	#serial_rx
	mov	IN,	DPL
	mov	IN+1,	DPH

	mov	dptr,	#serial_tx
	mov	OUT,	DPL
	mov	OUT+1,	DPH
