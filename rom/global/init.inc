
; *** global/init.inc ***

; Initialize stack pointer and global variables.
; This must be included early in the main program.

	mov	SP,	#STACK

	mov	dptr,	#serial_tx
	mov	COUT,	DPL
	mov	COUT+1, DPH

;	mov	dptr,	#serial_rx
;	mov	CIN,	DPL
;	mov	CIN+1,	DPH
