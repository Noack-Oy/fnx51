
; *** print/char.inc ***

; >> void print_char(char a) <<

; Print char from accumulator.
; Indirect call to function behind COUT pointer.

print_char:
	push	OUT
	push	OUT+1
	ret
