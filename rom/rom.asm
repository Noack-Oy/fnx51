
; *** headers ***

.inc global/variables.equ
.inc serial/sfr.equ

; *** interrupt vectors ***

.org 0x0000
	sjmp	main
.org 0x0050

; *** main program ***

main:

.inc global/init.inc

	acall	serial_init
	mov	dptr,#text
	acall	print_text
	jnz	halt


halt:
	sjmp	halt

text:
	.db	"Hello, world!", 13, 10, 0

; *** library ***

.inc serial/init.inc
.inc serial/rx.inc
.inc serial/tx.inc
.inc print/char.inc
.inc print/text.inc