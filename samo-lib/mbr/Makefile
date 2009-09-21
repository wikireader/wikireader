SAMO_LIB := $(shell readlink -es ../samo-lib || readlink -es ../../samo-lib || readlink -es ../../../samo-lib)
FATFS_MODE = read-only

include ${SAMO_LIB}/Mk/definitions.mk

# set this to 0 or 1 to control the battery indicator in menu.c
BATTERY_METER ?= 0


FLASH_ROM_MAP_FILE ?= SAMO_A1.mapfile
FLASH07 ?= ${HOST_TOOLS}/flash07/flash07
IMAGE07 ?= ${HOST_TOOLS}/flash07/image07
BOOT_FILE ?= ${HOST_TOOLS}/jackknife/jackknife

LIBS += ${FATFS_LIB}
LIBS += ${DRIVERS_LIB}
LIBS += ${MINI_LIBC_LIB}
LIBS += $(shell $(CC) -print-libgcc-file-name)

LDFLAGS = -static --strip-all -s --no-gc-sections -N -L$(MINI_LIBC)

INCLUDES = -I${FATFS_INCLUDE}
INCLUDES += -I${FATFS_CONFIG_INCLUDE}
INCLUDES += -I${MINI_LIBC_INCLUDE}
INCLUDES += -I${DRIVERS_INCLUDE}
INCLUDES += -I$(SAMO_LIB_INCLUDE)
INCLUDES += -I$(WIKI_APP_INCLUDE)

OBJCOPY_FLAGS = -I elf32-c33 -O binary

OBJCOPY_SECTIONS = --only-section=.text
OBJCOPY_SECTIONS += --only-section=.rodata
OBJCOPY_SECTIONS += --only-section=.data

TARGETS = $(FLASH_ROM_MAP_FILE) mbr serial.header
APPLICATIONS =
APPLICATION_CLEAN =

CFLAGS += -DBATTERY_METER="${BATTERY_METER}"

vpath %.c $(WIKI_APP_SRC)

.PHONY: all
all: $(TARGETS) all-applications flash.rom

# Master boot record - a multiple program boot loader

$(FLASH_ROM_MAP_FILE):
	cp SAMO_A1.mapfile-default $(FLASH_ROM_MAP_FILE)

flash.rom: ${FLASH_ROM_MAP_FILE} ${TARGETS} all-applications
	${IMAGE07} --image-file="$@" --map-file="${FLASH_ROM_MAP_FILE}"

mbr.elf: mbr.o
	$(GCC) -o $@ $(LDFLAGS) $^ -Wl,'-Ttext=0',-Map,$(@:.elf=.map) ${LIBS}

mbr: mbr.elf
	${RM} "$@"
	$(OBJCOPY) $(OBJCOPY_FLAGS) ${OBJCOPY_SECTIONS}	 --set-start=0 $< $@
	#$(OBJDUMP) --target=binary --architecture=c33 --disassemble-all $@ > $@.asm33
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

$(call MAKE_APP,menu,misc.o,splash.h empty.h)
$(call MAKE_APP,hello,misc.o)
$(call MAKE_APP,memory-test,misc.o)
$(call MAKE_APP,hw-test,misc.o)
$(call MAKE_APP,file-loader,misc.o elf32.o)
$(call MAKE_APP,rs232-loader,misc.o)
#$(call MAKE_APP,lcd-test,misc.o)

# application pre-depends

splash.h: splash.png
	$(HOST_TOOLS)/imagetool/imagetool.py splash.png splash.h splash_image

empty.h: empty.png
	$(HOST_TOOLS)/imagetool/imagetool.py empty.png empty.h empty_image

# rule to build all above applications

.PHONY: all-applications
all-applications: ${APPLICATIONS}

flash-mbr: all
	time ${FLASH07} --port="${BOOTLOADER_TTY}" --aux-port="${BOOTLOADER_AUX}" --boot="${BOOT_FILE}" \
	--map-file="${FLASH_ROM_MAP_FILE}"

.PHONY: always-build
serial.header: always-build
	( echo -n ${SERIAL_NUMBER} ; \
	  printf '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0'; \
	  printf '\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0'; \
	) | dd bs=1 count=32 > "$@"

clean:
	rm -rf ${TARGETS} ${APPLICATIONS_CLEAN} *.o *.elf *.d *.asm33 *.header *.map splash.h *.rom

# application headers
%.header: %.c
	awk -f GenerateApplicationHeader.awk "$<" >"$@"

include ${SAMO_LIB}/Mk/rules.mk
