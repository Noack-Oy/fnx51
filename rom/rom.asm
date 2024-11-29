.equ	cmod,	0xd9

.equ	ccon,	0xd8
.flag	cf,	ccon.7
.flag	cr,	ccon.6
.flag	ccf4,	ccon.4
.flag	ccf3,	ccon.3
.flag	ccf2,	ccon.2
.flag	ccf1,	ccon.1
.flag	ccf0,	ccon.0

.equ	ccapm0, 0xda
.equ	ccapm1, 0xdb
.equ	ccapm2, 0xdc
.equ	ccapm3, 0xdd
.equ	ccapm4, 0xde

.equ	ccap0l, 0xea
.equ	ccap1l, 0xeb
.equ	ccap2l, 0xec
.equ	ccap3l, 0xed
.equ	ccap4l, 0xee

.equ	ccap0h, 0xfa
.equ	ccap1h, 0xfb
.equ	ccap2h, 0xfc
.equ	ccap3h, 0xfd
.equ	ccap4h, 0xfe

.equ	cl,	0xe9
.equ	ch,	0xf9

.flag	ec,	ie.6

.org 0x0000
	sjmp	main
.org 0x0033
	ajmp	pca_int
.org 0x0050

main:
	mov	cmod,	#0x03	; set cps0 and ecf
	mov	ccapm0, #0x4c	; set tog, mat, ecom
	mov	ccap0l, #0x20
	mov	ccap0h, #0xf0
	setb	ec
	setb	ea
	setb	cr

loop:
	sjmp	loop

pca_int:
	setb	p1.3
	mov	ch,	#0xf0
	clr	cf
	reti
