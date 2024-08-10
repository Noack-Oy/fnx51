all: serial-autobaud.ser.md

serial-%.md: ../../rom/serial/%
	echo '```' > $@
	cat $< >> $@
	echo '```' >> $@
