LIBDIR = ../mini-libc
FATFSDIR = ../fatfs
FATFS_MODE = read-only
SAMO_LIB = ../../samo-lib
HOST_TOOLS = ../../host-tools
WIKILIB_DIR = ../../wiki-lib

FLASH_ROM_MAP_FILE ?= SAMO_A1.mapfile
E07LOAD ?= ${HOST_TOOLS}/e07load/e07load
RS232 ?= ${HOST_TOOLS}/jackknife/rs232

LIBS = $(FATFSDIR)/lib/${FATFS_MODE}/libtinyfat.a
LIBS += $(LIBDIR)/lib/libc.a
LIBS += $(shell $(CC) -print-libgcc-file-name)

LDFLAGS = -static --strip-all -s --no-gc-sections -N -L$(LIBDIR)

INCLUDES = -I$(FATFSDIR)/src
INCLUDES += -I$(LIBDIR)/include/
INCLUDES += -I$(FATFSDIR)/config/c33/${FATFS_MODE}/
INCLUDES += -I$(SAMO_LIB) -I$(WIKILIB_DIR)/include

OBJCOPY_FLAGS = -I elf32-c33 -O binary

OBJCOPY_SECTIONS = --only-section=.text
OBJCOPY_SECTIONS += --only-section=.rodata
OBJCOPY_SECTIONS += --only-section=.data

TARGETS = $(FLASH_ROM_MAP_FILE) mbr
APPLICATIONS =
APPLICATION_CLEAN =

VPATH = $(SAMO_LIB) $(WIKILIB_DIR)/generic $(WIKILIB_DIR)/specific

.PHONY: all
all: $(TARGETS) all-applications

# Master boot record - a multiple program boot loader

$(FLASH_ROM_MAP_FILE):
	cp SAMO_A1.mapfile-default $(FLASH_ROM_MAP_FILE)

mbr.o: mbr.c
	$(GCC) -c $(CFLAGS) $<

mbr.elf: mbr.o eeprom.o
	$(GCC) -o $@ $(LDFLAGS) $^ -Wl,'-Ttext=0'

mbr: mbr.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) ${OBJCOPY_SECTIONS}	 --set-start=0 $< $@
	$(OBJDUMP) --target=binary --architecture=c33 --disassemble-all $@ > $@.asm33
	@c=$$(wc --bytes < "$@") ; \
	echo mbr size = $${c} bytes, max = 512 bytes ; \
	if [ "$${c}" -gt 512 ]; then $(RM) "$@"; exit 99; fi

mbr-rs232: SAMO_A1.mapfile-rs232 mbr
	cp SAMO_A1.mapfile-rs232 $(FLASH_ROM_MAP_FILE)

# macro to create application rules
# $call MAKE_APP, app_name, file1.o file2.o, <optional pre-depends>)
# Note: 1. list of objects/pre-depends are _space_ separated
#       2. eeprom is removed as this is provided by mbr
#       3. strip and _NO_ spaces after comma (make passes all spaces to arguments)

MAKE_APP = $(eval $(call MAKE_APP1,$(strip ${1}),$(strip ${2}),$(strip ${3})))

define MAKE_APP1

APPLICATIONS += ${1}
APPLICATIONS_CLEAN += ${1} ${1}.elf ${1}.header

${1}.elf: ${3} application.lds ${1}.o $(filter-out eeprom.o,${2}) $$(LIBS)
	$$(LD) -T application.lds -o $$@ $$(LDFLAGS) ${1}.o $(filter-out eeprom.o,${2}) $$(LIBS) -Map $$(@:.elf=.map)

${1}: ${1}.elf ${1}.header
	$$(OBJCOPY) $$(OBJCOPY_FLAGS) $${OBJCOPY_SECTIONS} $$< $$@
endef

# Applications for the boot loader

$(call MAKE_APP,menu,misc.o lcd.o, splash.h)
$(call MAKE_APP,hello,misc.o)
$(call MAKE_APP,memory-test,misc.o)
$(call MAKE_APP,key-test,misc.o)
$(call MAKE_APP,forth-loader,misc.o elf32.o)
$(call MAKE_APP,kernel-loader,misc.o elf32.o)
$(call MAKE_APP,rs232-loader,misc.o)
$(call MAKE_APP,lcd-test,misc.o lcd.o)

# application pre-depends

splash.h: splash.png
	$(HOST_TOOLS)/imagetool/imagetool.py splash.png splash.h splash_image

# rule to build all above applications

.PHONY: all-applications
all-applications: ${APPLICATIONS}

flash-mbr: all
	time ${E07LOAD} --tty="${BOOTLOADER_TTY}" --bootstrap="${RS232}" "${FLASH_ROM_MAP_FILE}"

clean:
	rm -rf ${TARGETS} ${APPLICATIONS_CLEAN} *.o *.elf *.d *.asm33 *.header *.map splash.h

# application headers
%.header: %.c
	awk -f ../../scripts/GenerateApplicationHeader.awk "$<" >"$@"

include ../../samo-lib/Makefile
