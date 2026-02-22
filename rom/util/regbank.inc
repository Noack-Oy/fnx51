
; *** util/regbank.inc ***

; >> void*{r0} regbank_next() <<

; Switch to next register bank.
; Return pointer to r0 of old bank in new r0.

regbank_next:
	push	acc
	; check if already at highest bank
	mov	a,	psw
	anl	a,	#0x18	; mask off rs0 and rs1
	cjne	a,	#0x18,	regbank_next__continue
	acall	panic	; overflow
regbank_next__continue:
	; switch to next bank
	mov	a,	psw
	add	a,	#0x08
	mov	psw,	a
	; calculate pointer to old bank
	anl	a,	#0x18
	add	a,	#0xf8 ; subtract 8
	mov	r0,	a
	; done
	pop	acc
	ret

; >> void regbank_prev() <<

; Switch to previous register bank.

regbank_prev:
	push	acc
	; check if already at lowest bank
	mov	a,	psw
	anl	a,	#0x18
	jnz	regbank_prev__continue
	acall	panic	; underflow
regbank_prev__continue:
	; switch to previous bank
	mov	a,	psw
	add	a,	#0xf8 ; subtract 8
	mov	psw,	a
	; done
	pop	acc
	ret
