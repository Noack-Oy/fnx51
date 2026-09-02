SHELL=/bin/bash -o pipefail

sources = serial.asm print.asm panic.asm dump.asm \
          stream_imem.asm stream_xram.asm \
          read_hex.asm memory.asm sd.asm block.asm \
          fatfs.asm

# Set REMOTE_HOST to run tests on remote machine, leave unset for local execution
REMOTE_HOST ?= pi@192.168.1.159
# The router forwards 22078 to the Pi's wired link, 22077 to its wifi (the way
# back in after a USB controller wedge takes the wired NIC down). Keep the
# comment off the assignment line: make would fold the spaces before a trailing
# '#' into the value.
REMOTE_PORT ?= 22078
SSH := ssh -p $(REMOTE_PORT)
REMOTE_DIR = /tmp/fnx51-test

default: $(sources:.asm=.lst)
test: $(sources:.asm=.out)

include $(sources:.asm=.d)

%.lst %.hex: %.asm
	zinc51 $*.asm -l $*.lst

%.out: %.hex %.t.py
	rm -vf $*.fail
ifdef REMOTE_HOST
	@echo "Setting up test on remote host $(REMOTE_HOST)..."
	@$(SSH) $(REMOTE_HOST) 'mkdir -p $(REMOTE_DIR)'
	@rsync -au -e '$(SSH)' ../loader.py util.py $*.hex $*.t.py $(REMOTE_HOST):$(REMOTE_DIR)/
	@echo "Running test on remote host..."
	($(SSH) $(REMOTE_HOST) 'cd $(REMOTE_DIR) && python3 -u $*.t.py' | tee $*.out) || mv -v $*.out $*.fail
else
	@echo "Running test locally..."
	(python3 -u $*.t.py | tee $*.out) || mv -v $*.out $*.fail
endif

%.d: %.asm
	echo -n '$*.lst $*.hex: $*.asm ' > $@
	grep -i '^\.include' $< | awk '{print $$2}' | xargs >> $@

clean:
	rm -vf *.hex *.lst *.out *.fail
