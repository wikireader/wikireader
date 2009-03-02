LIBDIR=$(PWD)/../toolchain/mini-libc
FATFSDIR=$(PWD)/../fatfs
COMMON=$(PWD)/../common
LIBS=$(FATFSDIR)/lib/libtinyfat.a $(LIBDIR)/lib/libc.a `$(CC) -print-libgcc-file-name`
LDFLAGS=-static --strip-all -s --no-gc-sections -N -L$(LIBDIR)
INCLUDES=-I$(FATFSDIR)/src -I$(LIBDIR)/include/ -I$(FATFSDIR)/config/c33/ -I$(COMMON)
OBJCOPY_FLAGS=-I elf32-c33 -O binary 

TARGETS=rs232 eeprom-1st eeprom-2nd eeprom-2nd.bin e07load/e07load gfxtool/gfxtool
VPATH=$(COMMON)
all: $(TARGETS)

#### e07load
e07load/e07load::
	make -C e07load

#### gfxtool
gfxtool/gfxtool::
	make -C gfxtool

#### first boot loader
rs232.elf: rs232.o
	$(GCC) -o $@ $(LDFLAGS) $< -Wl,'-Ttext=0'

rs232: rs232.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text --set-start=0 $< $@

#### second boot loader
eeprom-1st.o: eeprom-1st.c eeprom-2nd
	$(GCC) -c $(CFLAGS) -DEEPROM_PAYLOAD_SIZE=`wc -c eeprom-2nd | cut -f1 -d' '` $<

eeprom-1st.elf: eeprom-1st.o eeprom.o 
	$(GCC) -o $@ $(LDFLAGS) $^ -Wl,'-Ttext=0'

eeprom-1st: eeprom-1st.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text --set-start=0 $< $@

#### third boot loader
eeprom-2nd.elf: eeprom-2nd.o misc.o eeprom.o elf32.o lcd.o eeprom-2nd.lds ../fatfs/lib/libtinyfat.a
	$(LD) -o $@ $(LDFLAGS) $^ $(LIBS) -T $(<:.o=.lds) -Map $(<:.o=.map)

eeprom-2nd: eeprom-2nd.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text	\
				--only-section=.rodata		\
				$< $@

eeprom-2nd.bin: eeprom-2nd.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text	\
				--only-section=.rodata		\
				--pad-to=8192			\
				$< $@

# flash target
flash: $(TARGETS)
	@e07load/e07load wikireader.map

clean::
	make -C e07load clean
	make -C gfxtool clean
	rm -fr $(TARGETS) *.o *.elf *.d eeprom-2nd.map

include ../common/Makefile
