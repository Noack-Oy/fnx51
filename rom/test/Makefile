SHELL=/bin/bash -o pipefail

sources = serial.asm

default: $(sources:.asm=.out)

include $(sources:.asm=.d)

%.lst %.hex: %.asm
	as31 -l $< || rm -vf $*.lst $*.hex

%.out: %.hex %.t.py
	rm -vf $*.fail
	(python3 $*.t.py | tee $*.out) || mv -v $*.out $*.fail

%.d: %.asm
	echo -n '$*.lst $*.hex: $*.asm ' > $@
	grep -i '^\.inc' $< | awk '{print $$2}' | xargs >> $@
