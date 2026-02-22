
; *** headers ***

.inc global/variables.equ
.inc serial/sfr.equ
.inc pca/sfr.equ


; *** interrupt vectors ***

.org 0x0000
	sjmp	main
.org 0x0033
	ajmp	pca_interrupt
.org 0x0050


; *** main program ***

main:

.inc global/init.inc

	acall	serial_init

	mov	cmod,	#0x03	; set cps0 and ecf
	mov	ccapm0, #0x4c	; set tog, mat, ecom
	mov	ccap0l, #0x20
	mov	ccap0h, #0xf0
	setb	ec
	setb	ea
	setb	cr

loop:
	acall	read_hex_32
	mov	ccap0l, r0
	mov	ccap0h, r1
	sjmp	loop

pca_interrupt:
	setb	p1.3
	mov	ch,	#0xf0
	clr	cf
	reti


; *** library ***

.inc serial/init.inc
.inc serial/rx.inc
.inc serial/tx.inc
.inc read/char.inc
.inc read/hex.inc
