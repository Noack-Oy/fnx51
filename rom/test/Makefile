SHELL=/bin/bash -o pipefail

sources = serial.asm print.asm

default: $(sources:.asm=.lst)
test: $(sources:.asm=.out)

include $(sources:.asm=.d)

%.lst %.hex: %.asm
	as31 -l $*.asm || (rm -vf $*.lst $*.hex ; exit 1)

%.out: %.hex %.t.py
	rm -vf $*.fail
	(python3 $*.t.py | tee $*.out) || mv -v $*.out $*.fail

%.d: %.asm
	echo -n '$*.lst $*.hex: $*.asm ' > $@
	grep -i '^\.inc' $< | awk '{print $$2}' | xargs >> $@
