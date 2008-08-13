CROSS=c33-epson-elf-
GCC=$(CROSS)gcc
AS=$(CROSS)as
LD=$(CROSS)ld
HOSTCC=gcc
CC=$(GCC)
OBJCOPY=$(CROSS)objcopy

LIBDIR=/home/daniel/src/axlinux-c33pe.061027/tools/c33/lib/gcc-lib/c33-epson-elf/3.3.2/33401/
LIBS=$(LIBDIR)/libgcc.a $(LIBDIR)/libc.a

LDFLAGS=-static -s --no-gc-sections -N -L$(LIBDIR)
CFLAGS=-Wall -I. -gstabs -mlong-calls -fno-builtin -Os #-medda32 -mc33adv
ASFLAGS=-c -xassembler-with-cpp -Wa,--gstabs -medda32 -mc33pe -mc33_ext
OBJCOPY_FLAGS=-I elf32-c33 -O binary 

TARGETS=rs232 eeprom-1st eeprom-2nd e07load/e07load
all: $(TARGETS)

.c.o:
	$(GCC) -M $(CFLAGS) $< > $(<:.c=.d)
	$(GCC) $(CFLAGS) -c $< -o $(<:.c=.o)

#### e07load
e07load/e07load::
	make -C e07load

#### first boot loader
rs232.elf: rs232.o
	$(GCC) -o $@ $(LDFLAGS) $< -Wl,'-Ttext=0'

rs232: rs232.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text --set-start=0 $< $@

#### second boot loader
eeprom-1st.o: eeprom-1st.c eeprom-2nd
	$(GCC) -c $(CFLAGS) -DEEPROM_PAYLOAD_SIZE=`wc -c eeprom-2nd | cut -f1 -d' '` $<

eeprom-1st.elf: eeprom-1st.o eeprom.o spi.o
	$(GCC) -o $@ $(LDFLAGS) $^ -Wl,'-Ttext=0'

eeprom-1st: eeprom-1st.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text --set-start=0 $< $@

#### third boot loader
eeprom-2nd.elf: eeprom-2nd.o misc.o spi.o sdcard.o crc.o fat.o eeprom.o
	$(LD) -o $@ $(LDFLAGS) $^ $(LIBS) -T $(<:.o=.lds) -Map $(<:.o=.map)

eeprom-2nd: eeprom-2nd.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text	\
				--only-section=.rodata		\
				$< $@

# flash target
flash: rs232.elf eeprom-1st.elf eeprom-2nd.elf
	@e07load/e07load wikireader.map

#--change-section-address .rodata=0x5000
#--set-start=0x10000000 --pad-to 30000

clean::
	make -C e07load clean
	rm -fr $(TARGETS) *.o *.elf *.d eeprom-2nd.map

-include $(wildcard *.d) dummy

