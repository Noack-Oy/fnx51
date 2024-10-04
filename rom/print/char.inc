
; print/char.inc

; >> void print_char(char a) <<

; Print char from accumulator.
; Indirect call to function behind COUT pointer.

print_char:
	push	COUT
	push	COUT+1
	ret
