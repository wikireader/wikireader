# Copyright (c) 2009 Openmoko Inc
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

define FIND_SAMO_LIB
d='.' ; \
while [ X"/" != X"$${d}" ] ; \
do \
  s="$${d}/samo-lib" ; \
  [ -d "$${s}" ] && echo $${s} && exit 0 ; \
  d=$$(readlink -es "$${d}/..") ; \
done ; \
echo samo_lib_NOT_FOUND ; \
exit 1
endef
SAMO_LIB := $(shell ${FIND_SAMO_LIB})

# to minimise code in boot loader only remove all write functions
# from file system code
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

IMAGE_HEADERS = splash.h empty.h adjust.h

$(call MAKE_APP,menu,misc.o,${IMAGE_HEADERS})
$(call MAKE_APP,hello,misc.o)
$(call MAKE_APP,memory-test,misc.o)
$(call MAKE_APP,hw-test,misc.o)
$(call MAKE_APP,file-loader,misc.o elf32.o)
$(call MAKE_APP,rs232-loader,misc.o)
#$(call MAKE_APP,lcd-test,misc.o)

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


.PHONY: install
install: all
	@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	${COPY} flash.rom "${DESTDIR}"


.PHONY: clean
clean:
	rm -rf ${TARGETS} ${APPLICATIONS_CLEAN} *.o *.elf *.d *.asm33 *.header *.map *.rom ${IMAGE_HEADERS}

# application headers
%.header: %.c
	awk -f GenerateApplicationHeader.awk "$<" >"$@"

# application images

%.h: %.png
	$(HOST_TOOLS)/imagetool/imagetool.py "$<" "$@" "${@:.h=_image}"

include ${SAMO_LIB}/Mk/rules.mk
