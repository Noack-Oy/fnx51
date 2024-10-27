
; *** read/char.inc ***

; >> char read_char() <<

; Read char into accumulator.
; Indirect call to function behind IN pointer.

read_char:
	push	IN
	push	IN+1
	ret
