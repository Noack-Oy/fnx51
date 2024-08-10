; serial/init.inc

serial_init:
orl	PCON,	#0x80	; SMOD1 (2x baud rate)
mov	SCON,	#0x50	; SM1, REN
mov	BRL,	#245	; 9600 @20MHz CLK
mov	BDRCON,	#0x0e	; RBCK, TBCK, SPD
