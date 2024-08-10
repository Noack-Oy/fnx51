default: hello.lst

%.lst %.hex: %.asm *.equ *.inc
	as31 -l $< || rm -vf $*.lst $*.hex
