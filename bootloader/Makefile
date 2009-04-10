LIBDIR=../toolchain/mini-libc
FATFSDIR=../fatfs
COMMON=../common
LIBS=$(FATFSDIR)/lib/libtinyfat.a $(LIBDIR)/lib/libc.a `$(CC) -print-libgcc-file-name`
LDFLAGS=-static --strip-all -s --no-gc-sections -N -L$(LIBDIR)
INCLUDES=-I$(FATFSDIR)/src -I$(LIBDIR)/include/ -I$(FATFSDIR)/config/c33/ -I$(COMMON)
OBJCOPY_FLAGS=-I elf32-c33 -O binary

TARGETS=rs232 eeprom-1st eeprom-2nd eeprom-2nd.bin e07load/e07load gfxtool/gfxtool
VPATH=$(COMMON)

BOOTLOADER_TTY ?= /dev/ttyUSB0
FLASH_ROM_MAP_FILE ?= wikireader.map

ifeq ($(MAKECMDGOALS),bootloader232)
EXTRA_CFLAGS = -DEEPROM1ST_LOAD_FROM_RS232
else
EXTRA_CFLAGS = -DEEPROM1ST_LOAD_FROM_EEPROM
endif

ifneq ($(MAKECMDGOALS),flash-bootloader)
$(shell [ -f last_bootfrom ] && [ -z "`grep '\$(EXTRA_CFLAGS)' last_bootfrom`" ] && rm last_bootfrom)
endif


bootloader:$(TARGETS)

bootloader232:bootloader


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
last_bootfrom:
	$(shell echo $(EXTRA_CFLAGS) > last_bootfrom)

eeprom-1st.o: eeprom-1st.c eeprom-2nd last_bootfrom
	$(GCC) -c $(CFLAGS) $(EXTRA_CFLAGS) -DEEPROM_PAYLOAD_SIZE=`wc -c eeprom-2nd | cut -f1 -d' '` $<

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


# Master boot record - a multiple program boot loader

mbr.o: mbr.c last_bootfrom
	$(GCC) -c $(CFLAGS) $<


mbr.elf: mbr.o eeprom.o
	$(GCC) -o $@ $(LDFLAGS) $^ -Wl,'-Ttext=0'

TARGETS += mbr
mbr: mbr.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text --set-start=0 $< $@
	$(OBJDUMP) --target=binary --architecture=c33 --disassemble-all $@ > $@.asm33

#### third boot loader


# macro to create application rules
# $call MAKE_APP, app_name, file1.o file2.o)
# Note: 1. list of objects are _space_ separated
#       2. eeprom is removed as this is provided by mbr

MAKE_APP = $(eval $(call MAKE_APP1, ${1}, ${2}))

define MAKE_APP1
TARGETS += ${1}
${1}.elf: ${1}.o $(filter-out eeprom.o,${2}) application.lds
	$$(LD) -o $$@ $$(LDFLAGS) $$^ $$(LIBS) -T application.lds -Map $$(@:.elf=.map)

${1}: ${1}.elf ${1}.header
	$$(OBJCOPY) $$(OBJCOPY_FLAGS) --only-section=.text	\
				--only-section=.rodata		\
				$$< $$@
endef

# Applications

$(call MAKE_APP, menu, misc.o)
$(call MAKE_APP, hello, misc.o)
$(call MAKE_APP, memory-test, misc.o)
$(call MAKE_APP, key-test, misc.o)
$(call MAKE_APP, forth-loader, misc.o elf32.o)
$(call MAKE_APP, kernel-loader, misc.o elf32.o)


# flash target
flash-bootloader: $(TARGETS)
	time e07load/e07load --tty="${BOOTLOADER_TTY}" "${FLASH_ROM_MAP_FILE}"

clean:
	make -C e07load clean
	make -C gfxtool clean
	rm -fr $(TARGETS) *.o *.elf *.bin *.d eeprom-2nd.map last_bootfrom *.asm33 *.header

include ../common/Makefile

# application headers
%.header: %.c
	awk -f ../scripts/GenerateApplicationHeader.awk "$<" >"$@"
