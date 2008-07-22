CROSS=c33-epson-elf-
GCC=$(CROSS)gcc
CC=$(GCC)
LDFLAGS=-static -s -Wl,'-e 0' -Wl,'-z origin' -Wl,'-Ttext=0' -r --gc-sections -N -Ttext=0
CFLAGS=-I.
OBJCOPY=$(CROSS)objcopy
OBJCOPY_FLAGS=-I elf32-c33 -O binary --set-start=0

TARGETS=rs232 eeprom.img
all: $(TARGETS)


rs232.elf: rs232.o
	$(GCC) -o $@ $(LDFLAGS) $<

eeprom-1st.elf: eeprom-1st.o
	$(GCC) -o $@ $(LDFLAGS) $<

eeprom-2nd.elf: eeprom-2nd.o
	$(GCC) -o $@ $(LDFLAGS) $<

rs232: rs232.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) $< $@

eeprom-1st: eeprom-1st.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) $< $@

eeprom-2nd: eeprom-2nd.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) $< $@

eeprom.img: eeprom-1st eeprom-2nd
	dd if=eeprom-1st of=$@ bs=512 count=1
	dd if=eeprom-2nd of=$@ bs=512 seek=1

clean::
	rm -fr $(TARGETS) *.o *.elf


