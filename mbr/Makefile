LIBDIR = ../toolchain/mini-libc
FATFSDIR = ../fatfs
COMMON = ../common
LIBS = $(FATFSDIR)/lib/libtinyfat.a $(LIBDIR)/lib/libc.a `$(CC) -print-libgcc-file-name`
LDFLAGS = -static --strip-all -s --no-gc-sections -N -L$(LIBDIR)
INCLUDES = -I$(FATFSDIR)/src -I$(LIBDIR)/include/ -I$(FATFSDIR)/config/c33/ -I$(COMMON) -I../bootloader
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


# macro to create application rules
# $call MAKE_APP, app_name, file1.o file2.o)
# Note: 1. list of objects are _space_ separated
#       2. eeprom is removed as this is provided by mbr
#       3. strip and _NO_ spaces after comma (make passes all spaces to arguments)

MAKE_APP = $(eval $(call MAKE_APP1,$(strip ${1}),${2}))


define MAKE_APP1

APPLICATIONS += ${1}
APPLICATIONS_CLEAN += ${1} ${1}.elf ${1}.header

${1}.elf: ${1}.o $(filter-out eeprom.o,${2}) application.lds
	$$(LD) -o $$@ $$(LDFLAGS) $$^ $$(LIBS) -T application.lds -Map $$(@:.elf=.map)

${1}: ${1}.elf ${1}.header
	$$(OBJCOPY) $$(OBJCOPY_FLAGS) --only-section=.text	\
				--only-section=.rodata		\
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


clean:
	rm -rf ${TARGETS} ${APPLICATIONS_CLEAN} *.o *.elf *.d *.asm33 *.header *.map

# application headers
%.header: %.c
	awk -f ../scripts/GenerateApplicationHeader.awk "$<" >"$@"


include ../common/Makefile
