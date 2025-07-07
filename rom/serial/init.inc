
; *** serial/init.inc ***

; >> void serial_init() <<

; Initialize serial interface.
; - assumes 20MHz clock and X2 mode
; - uses internal baud rate generator
; - speed: 9600 bits per second
; Formula: BRL = 256 - (F_clk / (32 * Baud_Rate))
; ---
; See datasheet page 52 for details,
; also note oscillator diagram on page 16!

serial_init:
	mov	SCON,	#0x52	; SM1, REN, TI
	mov	BRL,	#191	; 9600 @20MHz CLK
	mov	BDRCON, #0x1e	; BRR, RBCK, TBCK, SPD

	mov	dptr,	#serial_rx
	mov	IN,	DPL
	mov	IN+1,	DPH

	mov	dptr,	#serial_tx
	mov	OUT,	DPL
	mov	OUT+1,	DPH

	ret
