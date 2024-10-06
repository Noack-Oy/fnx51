
; ***********************
; * Output Test Program *
; ***********************

; This program tests the print_* functions.

; text:    "Hello, World!"
; hex_8:   hex numbers 00..ff
; hex_16:  hex numbers 0000..ffff in steps of 55 (hex)
; hex_32:  fibonacci sequence (in hex)
; int_u8:  all integers from 0 to 255
; int_s8:  all integers from -128 to 127
; int_u16: integers from 0 to 65535 in steps of 51 (decimal)
; int_s16: start at -32768 and add 127 until signed overflow
; int_u32: fibonacci sequence (in decimal)
; int_s32: start at -2147483648 and add 123456789 repeatedly


; **********************
; * Header Definitions *
; **********************
.inc ../global/variables.equ
.inc ../serial/sfr.equ


; *************
; * Main Code *
; *************

	.org	0
.inc	../global/init.inc

; *** test/print.asm ***

	acall	serial_init

; test print_text
	mov	dptr,	#test__text
	acall	print_text
	mov	dptr,	#test__newline
	acall	print_text

; test print_hex_8
	mov	a,	#0
test_hex_8__loop:
	acall	print_hex_8
	add	a,	#1
	jb	AC,	test_hex_8__newline
	mov	dptr,	#test__space
	acall	print_text
	sjmp	test_hex_8__loop
test_hex_8__newline:
	mov	dptr,	#test__newline
	acall	print_text
	jnz	test_hex_8__loop
test_hex_8__end:
	acall	print_text

; test print_hex_16
	mov	r0,	#0
	mov	r1,	#0
	mov	r3,	#12
test_hex_16__loop:
	acall	print_hex_16
	mov	a,	r0
	add	a,	#0x55
	mov	r0,	a
	mov	a,	r1
	addc	a,	#0
	mov	r1,	a
	jc	test_hex_16__end
	djnz	r3,	test_hex_16__space
	mov	r3,	#12
	mov	dptr,	#test__newline
	acall	print_text
	sjmp	test_hex_16__loop
test_hex_16__space:
	mov	dptr,	#test__space
	acall	print_text
	sjmp	test_hex_16__loop
test_hex_16__end:
	mov	dptr,	#test__newline
	acall	print_text
	acall	print_text

; test print_hex_32
	mov	dptr,	#test__newline
	mov	a,	#0
	mov	r0,	a
	mov	r1,	a
	mov	r2,	a
	mov	r3,	a
	mov	r4,	#1
	mov	r5,	a
	mov	r6,	a
	mov	r7,	a
test_hex_32__loop:
	acall	print_hex_32
	acall	print_text
	; r0-3 += r4-7
	mov	a,	r0
	add	a,	r4
	mov	r0,	a
	mov	a,	r1
	addc	a,	r5
	mov	r1,	a
	mov	a,	r2
	addc	a,	r6
	mov	r2,	a
	mov	a,	r3
	addc	a,	r7
	mov	r3,	a
	; swap r0-3 with r4-7
	mov	a,	r4
	xch	a,	r0
	mov	r4,	a
	mov	a,	r5
	xch	a,	r1
	mov	r5,	a
	mov	a,	r6
	xch	a,	r2
	mov	r6,	a
	mov	a,	r7
	xch	a,	r3
	mov	r7,	a
	jnc	test_hex_32__loop
test_hex_32__end:
	acall	print_hex_32
	acall	print_text
	acall	print_text

; test print_int_u8
	mov	a,	#0
test_int_u8__loop:
	acall	print_int_u8
	add	a,	#1
	jb	AC,	test_int_u8__newline
	mov	dptr,	#test__space
	acall	print_text
	sjmp	test_int_u8__loop
test_int_u8__newline:
	mov	dptr,	#test__newline
	acall	print_text
	jnz	test_int_u8__loop
test_int_u8__end:
	acall	print_text

; test print_int_s8
	mov	a,	#0x80
test_int_s8__loop:
	acall	print_int_s8
	add	a,	#1
	jb	OV,	test_int_s8__end
	jb	AC,	test_int_s8__newline
	mov	dptr,	#test__space
	acall	print_text
	sjmp	test_int_s8__loop
test_int_s8__newline:
	mov	dptr,	#test__newline
	acall	print_text
	sjmp	test_int_s8__loop
test_int_s8__end:
	mov	dptr,	#test__newline
	acall	print_text
	acall	print_text

; test print_hex_u16
	mov	r0,	#0
	mov	r1,	#0
	mov	r3,	#10
test_int_u16__loop:
	acall	print_int_u16
	mov	a,	r0
	add	a,	#51
	mov	r0,	a
	mov	a,	r1
	addc	a,	#0
	mov	r1,	a
	jc	test_int_u16__end
	djnz	r3,	test_int_u16__space
	mov	r3,	#10
	mov	dptr,	#test__newline
	acall	print_text
	sjmp	test_int_u16__loop
test_int_u16__space:
	mov	dptr,	#test__space
	acall	print_text
	sjmp	test_int_u16__loop
test_int_u16__end:
	mov	dptr,	#test__newline
	acall	print_text
	acall	print_text

; test print_hex_s16
	mov	r0,	#0
	mov	r1,	#0x80
	mov	r3,	#10
test_int_s16__loop:
	acall	print_int_s16
	mov	a,	r0
	add	a,	#127
	mov	r0,	a
	mov	a,	r1
	addc	a,	#0
	mov	r1,	a
	jb	OV,	test_int_s16__end
	djnz	r3,	test_int_s16__space
	mov	r3,	#10
	mov	dptr,	#test__newline
	acall	print_text
	sjmp	test_int_s16__loop
test_int_s16__space:
	mov	dptr,	#test__space
	acall	print_text
	sjmp	test_int_s16__loop
test_int_s16__end:
	mov	dptr,	#test__newline
	acall	print_text
	acall	print_text

; the end
	ajmp	*

test__text:
	.byte	"Hello, world!"
test__newline:
	.byte	13, 10, 0
test__space:
	.byte	32, 0

; *********************
; * Library Functions *
; *********************
.inc ../serial/init.inc
.inc ../serial/tx.inc
.inc ../print/char.inc
.inc ../print/text.inc
.inc ../print/hex.inc
.inc ../print/int.inc
