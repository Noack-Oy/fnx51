
; *** headers ***

.include global/variables.equ
.include serial/sfr.equ

; *** interrupt vectors ***

.org 0x0000
	sjmp	main
.org 0x0050

; *** main program ***

main:

.include global/init.inc

	acall	serial_init
	mov	dptr,#text
	acall	print_text
	jnz	halt


halt:
	sjmp	halt

text:
	.db	"Hello, world!", 13, 10, 0

; *** library ***

.include serial/init.inc
.include serial/rx.inc
.include serial/tx.inc
.include print/char.inc
.include print/text.inc