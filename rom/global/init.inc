
; *** global/init.inc ***

; Initialize stack pointer and global variables.
; This must be included early in the main program.

global_init:
	mov	SP,#STACK

; Select register bank zero, clear processor flags
	mov	PSW,#0

; Fill internal RAM with markers for uninitialized data
	mov	r0,#0 ; write pointer

	mov	a,#0x3f ; '?' for uninitialized registers
global_init__1:
	inc	r0 ; first iteration skips address 0, r0 lives there
	mov	@r0,a
	cjne	r0,#0x1f,global_init__1

	mov	a,#0x2a ; '*' for uninitialized variables
global_init__2:
	inc	r0
	mov	@r0,a
	cjne	r0,#STACK-1,global_init__2

	mov	a,#0x7e ; '~' for stack and above
global_init__3:
	inc	r0
	mov	@r0,a
	cjne	r0,#0xff,global_init__3
