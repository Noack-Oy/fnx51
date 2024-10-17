
; *** util/xch.inc ***

; >> void xch_r0123_r4567(uint32 &r0r1r2r3, uint32 &r4r5r6r7) <<

; Exchange registers r0-r3 with r4-r7.

xch_r0123_r4567:
	; r0 <-> r4
	xch	a,	r0
	xch	a,	r4
	xch	a,	r0
	; r1 <-> r5
	xch	a,	r1
	xch	a,	r5
	xch	a,	r1
	; r2 <-> r6
	xch	a,	r2
	xch	a,	r6
	xch	a,	r2
	; r3 <-> r7
	xch	a,	r3
	xch	a,	r7
	xch	a,	r3
	; a remains unchanged
	ret
