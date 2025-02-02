
; *** headers ***

.inc global/variables.equ
.inc serial/sfr.equ
.inc spi/sfr.equ

; *** interrupt vectors ***

.org 0x0000
	sjmp	main
.org 0x0050

; *** main program ***

main:

.inc global/init.inc

	acall	serial_init

; SPCON register configuration:
; .-----.-------.-------.---------------------------------------.
; | bit | name	| value | remark				|
; |-----|-------|-------|---------------------------------------|
; | 7	| SPR2	| 1	| SPR2:0 = 101-> Fclk_periph / 64	|
; | 6	| SPEN	| 1	| Set to enable the SPI			|
; | 5	| SSDIS | 1	| Set to disable /SS input		|
; | 4	| MSTR	| 1	| Set to configure the SPI as a master	|
; | 3	| CPOL	| 0	| Cleared to have SCK idle at '0'	|
; | 2	| CPHA	| 0	| Cleared to sample on leading edge	|
; | 1	| SPR1	| 0	| (see above)				|
; | 0	| SPR0	| 1	| (see above)	speed: 312.5 kHz	|
; '-----'-------'-------'---------------------------------------'

	mov	spcon,#0xf1

; 80 warmup pulses
	setb	p1.1
	mov	r0,#10
init_sync:
	mov	a,#0xff
	acall	spi_transfer
	djnz	r0,init_sync

	acall	sd_select

; initialization sequence
	acall	cmd0
	acall	cmd8
	acall	cmd58
	mov	b,#20
init_loop:
	acall	acmd41
	jz	init_done
	acall	delay
	djnz	b,init_loop
	sjmp	abort
init_done:
	acall	cmd58

; read a block
	mov	a,#17
	mov	r0,#0x00 ; block address
	mov	r1,#0x00
	mov	r2,#0x00
	mov	r3,#0x00
	mov	r4,#0x00 ; crc, ignored
	acall	sd_command
read_wait:
	acall	sd_read_response1
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

delay:
	push	acc
	push	b
	clr	a
	mov	b,a
delay__loop:
	nop
	nop
	nop
	nop
	nop
	djnz	acc,delay__loop
	djnz	b,delay__loop
	pop	b
	pop	acc
	ret

; CMD0: go to idle state
cmd0:
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	r4,#0x94
	acall	sd_command
	acall	sd_read_response1
	acall	print_hex_8
	acall	newline
	ret

; CMD8: send interface condition
cmd8:
	mov	a,#8
	mov	r0,#0xaa
	mov	r1,#0x01
	mov	r2,#0x00
	mov	r3,#0x00
	mov	r4,#0x86
	acall	sd_command
	acall	sd_read_response7
	acall	print_hex_8
	acall	print_hex_32
	acall	newline
	ret

; CMD58: read operation conditions register
cmd58:
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	r4,a
	mov	a,#58
	acall	sd_command
	acall	sd_read_response3
	acall	print_hex_8
	acall	print_hex_32
	acall	newline
	ret

; CMD55: application specific command (prefix)
cmd55:
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,a
	mov	r4,a
	mov	a,#55
	acall	sd_command
	acall	sd_read_response1
	acall	print_hex_8
	ret

; ACMD41: send operation condition
acmd41:
	acall	cmd55
	clr	a
	mov	r0,a
	mov	r1,a
	mov	r2,a
	mov	r3,#0x40 ; indicates high capacity support
	mov	r4,a
	mov	a,#41
	acall	sd_command
	acall	sd_read_response1
	acall	print_hex_8
	acall	newline
	ret

newline:
	push	acc
	mov	a,#13
	acall	print_char
	mov	a,#10
	acall	print_char
	pop	acc
	ret

; >> void sd_select() <<
; Activate the SD card
sd_select:
	push	acc
	mov	a,#0xff
	acall	spi_transfer
	clr	p1.1
	mov	a,#0xff
	acall	spi_transfer
	pop	acc
	ret

; >> void sd_deselect() <<
; Deactivate the SD card
sd_deselect:
	push	acc
	mov	a,#0xff
	acall	spi_transfer
	setb	p1.1
	mov	a,#0xff
	acall	spi_transfer
	pop	acc
	ret

; >> void sd_command(char{a} cmd, uint32{r0-r3} arg, uint8{r4} crc) <<
; Issue a command to the SD card
sd_command:
	; transmit command
	orl	a,#0x40	; transmission bit
	acall	spi_transfer
	; transmit argument
	mov	a,r3
	acall	spi_transfer
	mov	a,r2
	acall	spi_transfer
	mov	a,r1
	acall	spi_transfer
	mov	a,r0
	acall	spi_transfer
	; transmit crc
	mov	a,r4
	orl	a,#0x01	; end bit
	acall	spi_transfer
	ret

; >> uint8{a} sd_read_response1()
; Read response from SD card in 'R1' format into a
sd_read_response1:
	push	b
	mov	b,8	; number of tries
sd_read_response1__loop:
	mov	a,#0xff
	acall	spi_transfer
	cjne	a,#0xff,sd_read_response1__end	; success
	djnz	b,sd_read_response1__loop	; retry / timeout
sd_read_response1__end:
	pop	b
	ret

; >> uint8{a}, uint32{r0-r3} sd_read_response3()
; Read response from SD card in 'R3' format into a and r0-r3
sd_read_response3: ; same as R7
; >> uint8{a}, uint32{r0-r3} sd_read_response7()
; Read response from SD card in 'R7' format into a and r0-r3
sd_read_response7:
	acall	sd_read_response1
	cjne	a,#0x02,sd_read_response7__1
sd_read_response7__1:
	jc	sd_read_response7__2
	ret	; error in R1 response
sd_read_response7__2:
	mov	r0,a
	mov	a,#0xff
	acall	spi_transfer
	mov	r3,a
	mov	a,#0xff
	acall	spi_transfer
	mov	r2,a
	mov	a,#0xff
	acall	spi_transfer
	mov	r1,a
	mov	a,#0xff
	acall	spi_transfer
	xch	a,r0
	ret


; *** library ***

.inc spi/transfer.inc
.inc serial/init.inc
.inc serial/rx.inc
.inc serial/tx.inc
.inc print/char.inc
.inc print/hex.inc
