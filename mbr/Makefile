LIBDIR = ../toolchain/mini-libc
FATFSDIR = ../fatfs
FATFS_MODE = read-only
COMMON = ../common
BOOTLOADER = ../bootloader

FLASH_ROM_MAP_FILE ?= SAMO_A1.mapfile
E07LOAD ?= ${BOOTLOADER}/e07load/e07load
RS232 ?= ${BOOTLOADER}/rs232

LIBS = $(FATFSDIR)/lib/${FATFS_MODE}/libtinyfat.a
LIBS += $(LIBDIR)/lib/libc.a
LIBS += $(shell $(CC) -print-libgcc-file-name)

LDFLAGS = -static --strip-all -s --no-gc-sections -N -L$(LIBDIR)

INCLUDES = -I$(FATFSDIR)/src
INCLUDES += -I$(LIBDIR)/include/
INCLUDES += -I$(FATFSDIR)/config/c33/${FATFS_MODE}/
INCLUDES += -I$(COMMON) -I../bootloader

OBJCOPY_FLAGS = -I elf32-c33 -O binary
TARGETS = mbr
APPLICATIONS =
APPLICATION_CLEAN =

VPATH = $(COMMON):../bootloader

.PHONY: all
all: $(TARGETS) all-applications

# Master boot record - a multiple program boot loader

mbr.o: mbr.c
	$(GCC) -c $(CFLAGS) $<


mbr.elf: mbr.o eeprom.o
	$(GCC) -o $@ $(LDFLAGS) $^ -Wl,'-Ttext=0'

mbr: mbr.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text --set-start=0 $< $@
	$(OBJDUMP) --target=binary --architecture=c33 --disassemble-all $@ > $@.asm33
	@c=$$(wc --bytes < "$@") ; \
	echo mbr size = $${c} bytes, max = 512 bytes ; \
	[ "$${c}" -le 512 ] || exit 1

# macro to create application rules
# $call MAKE_APP, app_name, file1.o file2.o)
# Note: 1. list of objects are _space_ separated
#       2. eeprom is removed as this is provided by mbr
#       3. strip and _NO_ spaces after comma (make passes all spaces to arguments)

MAKE_APP = $(eval $(call MAKE_APP1,$(strip ${1}),${2}))


define MAKE_APP1

APPLICATIONS += ${1}
APPLICATIONS_CLEAN += ${1} ${1}.elf ${1}.header

${1}.elf: application.lds ${1}.o $(filter-out eeprom.o,${2}) $$(LIBS)
	$$(LD) -T application.lds -o $$@ $$(LDFLAGS) $$^ $$(LIBS) -Map $$(@:.elf=.map)

${1}: ${1}.elf ${1}.header
	$$(OBJCOPY) $$(OBJCOPY_FLAGS) --only-section=.text	\
				--only-section=.rodata		\
				--only-section=.data		\
				$$< $$@
endef


# Applications for the boot loader

$(call MAKE_APP,menu,misc.o)
$(call MAKE_APP,hello,misc.o)
$(call MAKE_APP,memory-test,misc.o)
$(call MAKE_APP,key-test,misc.o)
$(call MAKE_APP,forth-loader,misc.o elf32.o)
$(call MAKE_APP,kernel-loader,misc.o elf32.o)


# rule to build all above applications

.PHONY: all-applications
all-applications: ${APPLICATIONS}


flash-mbr: all
	time ${E07LOAD} --tty="${BOOTLOADER_TTY}" --bootstrap="${RS232}" "${FLASH_ROM_MAP_FILE}"


clean:
	rm -rf ${TARGETS} ${APPLICATIONS_CLEAN} *.o *.elf *.d *.asm33 *.header *.map

# application headers
%.header: %.c
	awk -f ../scripts/GenerateApplicationHeader.awk "$<" >"$@"


include ../common/Makefile
