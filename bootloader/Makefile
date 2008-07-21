CROSS=c33-epson-elf-
GCC=$(CROSS)gcc
CC=$(GCC)
LDFLAGS=-static -s -Wl,'-e 0' -Wl,'-z origin' -Wl,'-Ttext=0' -r --gc-sections -N -Ttext=0
CFLAGS=-I.
OBJCOPY=$(CROSS)objcopy
OBJCOPY_FLAGS=-I elf32-c33 -O binary --set-start=0

TARGETS=rs232
all: $(TARGETS)


rs232.elf: main.o
	$(GCC) -o $@ $(LDFLAGS) $<

rs232: rs232.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) $< $@

clean::
	rm -fr $(TARGETS) *.o *.elf


