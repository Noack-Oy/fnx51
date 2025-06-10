
; *** headers ***

.inc global/variables.equ
.inc serial/sfr.equ
.inc spi/sfr.equ
.inc sd/config.equ

; *** interrupt vectors ***

.org 0x0000
	sjmp	main
.org 0x0050

; *** main program ***

main:

.inc global/init.inc

	acall	serial_init
	acall	sd_init
	acall	print_hex_8
	acall	print_hex_32
	acall	newline

; read a block
	mov	a,#17
	mov	r0,#0x00 ; block address
	mov	r1,#0x00
	mov	r2,#0x00
	mov	r3,#0x00
	mov	r4,#0x00 ; crc, ignored
	acall	sd_command
read_wait:
	acall	sd_response1
	acall	print_hex_8
	cjne	a,#0xfe,read_wait
	acall	newline
	mov	r0,#0
read_loop: ; 512 bytes (256*2)
	mov	a,#0xff
	acall	spi_transfer
	acall	print_hex_8
	mov	a,#0xff
	acall	spi_transfer
	acall	print_hex_8
	djnz	r0,read_loop
	acall	newline
; crc
	mov	a,#0xff
	acall	spi_transfer
	acall	print_hex_8
	mov	a,#0xff
	acall	spi_transfer
	acall	print_hex_8
	acall	newline

abort:
	acall	sd_deselect

halt:
	sjmp	halt

newline:
	push	acc
	mov	a,#13
	acall	print_char
	mov	a,#10
	acall	print_char
	pop	acc
	ret

; *** library ***

.inc spi/transfer.inc
.inc sd/init.inc
.inc sd/warmup.inc
.inc sd/select.inc
.inc sd/deselect.inc
.inc sd/command.inc
.inc sd/response.inc
.inc sd/cmd0.inc
.inc sd/cmd8.inc
.inc sd/cmd55.inc
.inc sd/cmd58.inc
.inc sd/acmd41.inc
.inc serial/init.inc
.inc serial/rx.inc
.inc serial/tx.inc
.inc print/char.inc
.inc print/hex.inc
.inc print/text.inc
.inc util/delay.inc
.inc util/regbank.inc
.equ panic_out, serial_tx
.inc util/panic.inc
