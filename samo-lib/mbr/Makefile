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

# to minimise code in boot loader only remove all write functions
# from file system code
FATFS_MODE = read-only

# +++START_UPDATE_MAKEFILE: Start of auto included code
# The text between the +++ and --- tags is copied by the
# UpdateMakefiles script. Do not remove or change these tags.
# ---
# Autodetect root directory
define FIND_ROOT_DIR
while : ; do \
  d=$$(pwd) ; \
  [ -d "$${d}/samo-lib" ] && echo $${d} && exit 0 ; \
  [ X"/" = X"$${d}" ] && echo ROOT_DIRECTORY_NOT_FOUND && exit 1 ; \
  cd .. ; \
done
endef
ROOT_DIR := $(shell ${FIND_ROOT_DIR})
# Directory of Makefile includes
MK_DIR   := ${ROOT_DIR}/samo-lib/Mk
# Include the initial Makefile setup
include ${MK_DIR}/definitions.mk
# ---END_UPDATE_MAKEFILE: End of auto included code


# set this to 0 or 1 to control the battery indicator in menu.c
BATTERY_METER ?= 0

# use update YES or NO
# YES => only change serial number if mbr version is ok
# NO  => full update even if this will downgrade the mbr

FLASH_ROM_MAP_FILE ?= SAMO_A1.mapfile
FLASH07 ?= ${HOST_TOOLS}/flash07/flash07
IMAGE07 ?= ${HOST_TOOLS}/flash07/image07

ifeq (YES,$(strip ${FLASH_TEST_JIG}))
BOOT_FILE ?= ${HOST_TOOLS}/jackknife/jakebrake
else
BOOT_FILE ?= ${HOST_TOOLS}/jackknife/jackknife
endif

LIBS += ${DRIVERS_LIB}
LIBS += ${FATFS_LIB}
LIBS += ${DRIVERS_LIB}
LIBS += ${MINI_LIBC_LIB}
LIBS += $(shell ${CC} -print-libgcc-file-name)

LDFLAGS = -static --strip-all -s --no-gc-sections -N -L${MINI_LIBC}

INCLUDES = -I${FATFS_INCLUDE}
INCLUDES += -I${FATFS_CONFIG_INCLUDE}
INCLUDES += -I${MINI_LIBC_INCLUDE}
INCLUDES += -I${DRIVERS_INCLUDE}
INCLUDES += -I${SAMO_LIB_INCLUDE}

OBJCOPY_FLAGS = -I elf32-c33 -O binary

OBJCOPY_SECTIONS = --only-section=.text
OBJCOPY_SECTIONS += --only-section=.rodata
OBJCOPY_SECTIONS += --only-section=.data

TARGETS = ${FLASH_ROM_MAP_FILE} mbr serial.header
APPLICATIONS =
APPLICATION_CLEAN =

CFLAGS += -DBATTERY_METER="${BATTERY_METER}"

vpath %.c

.PHONY: all
all: ${TARGETS} all-applications flash.rom

# Master boot record - a multiple program boot loader

${FLASH_ROM_MAP_FILE}:
	cp SAMO_A1.mapfile-default ${FLASH_ROM_MAP_FILE}

flash.rom: ${FLASH_ROM_MAP_FILE} ${TARGETS} all-applications
	${IMAGE07} --image-file="$@" --map-file="${FLASH_ROM_MAP_FILE}"

mbr.elf: mbr.o
	${CC} -o $@ ${LDFLAGS} $^ -Wl,'-Ttext=0',-Map,${@:.elf=.map}
	${OBJDUMP} --disassemble-all $@ > ${@:.elf=.dump}

mbr: mbr.elf
	${RM} "$@"
	${OBJCOPY} ${OBJCOPY_FLAGS} ${OBJCOPY_SECTIONS}	 --set-start=0 $< $@
	@c=$$(wc --bytes < "$@") ; \
	echo mbr size = $${c} bytes, max = 512 bytes ; \
	if [ "$${c}" -gt 512 ]; then ${RM} "$@"; exit 99; fi

mbr-rs232: SAMO_A1.mapfile-rs232 mbr
	cp SAMO_A1.mapfile-rs232 ${FLASH_ROM_MAP_FILE}


# macro to create application rules
# $call MAKE_APP, app_name, file1.o file2.o, <optional pre-depends>)
# Note: 1. list of objects/pre-depends are _space_ separated
#       2. eeprom is removed as this is provided by mbr
#       3. strip and _NO_ spaces after comma (make passes all spaces to arguments)

MAKE_APP = $(eval $(call MAKE_APP1,$(strip ${1}),$(strip ${2}),$(strip ${3})))

define MAKE_APP1

APPLICATIONS += ${1}
APPLICATIONS_CLEAN += ${1} ${1}.elf ${1}.header

${1}.elf: ${2} application.lds ${1}.o $${LIBS}
	$${LD} -T application.lds -o $$@ $${LDFLAGS} ${1}.o $${LIBS} -Map $${@:.elf=.map}
	$${OBJDUMP} --disassemble-all $$@ > $${@:.elf=.dump}

${1}: ${1}.elf ${1}.header
	$${OBJCOPY} $${OBJCOPY_FLAGS} $${OBJCOPY_SECTIONS} $$< $$@

endef

# Applications for the boot loader

IMAGE_HEADERS = splash.h empty.h adjust.h

$(call MAKE_APP,version)
$(call MAKE_APP,menu,${IMAGE_HEADERS})
$(call MAKE_APP,hello)
$(call MAKE_APP,memory-test)
$(call MAKE_APP,hw-test)
$(call MAKE_APP,file-loader)
$(call MAKE_APP,rs232-loader)

# rule to build all above applications

.PHONY: all-applications
all-applications: ${APPLICATIONS}

ifeq (YES,$(strip ${FLASH_UPDATE}))
UPDATE_ARGS = --update
else
UPDATE_ARGS =
endif

flash-mbr: all
	time ${FLASH07} --port="${BOOTLOADER_TTY}" --aux-port="${BOOTLOADER_AUX}" --boot="${BOOT_FILE}" \
	--map-file="${FLASH_ROM_MAP_FILE}" ${UPDATE_ARGS}

# serial number must be padded with 0xFF as that is
# the value of erased FLASH memory bytes
# Note: the dd commands below are required to pad and truncate
#       the serial number correctly
#       the swapped bs and count are intentional as the () code
#	produces two writes and the final dd takes just 32 bytes
.PHONY: always-build
serial.header: always-build
	( echo -n ${SERIAL_NUMBER} ; \
	  dd bs=32 count=1 if=/dev/zero | tr '\0' '\377' ; \
	) | dd bs=1 count=32 > "$@"


.PHONY: install
install: all
	@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	${COPY} flash.rom "${DESTDIR}"


.PHONY: clean
clean:
	${RM} ${TARGETS} ${APPLICATIONS_CLEAN} ${IMAGE_HEADERS}
	${RM} *.o *.elf *.d *.p *.asm33 *.header *.map *.rom *.dump

# application headers
%.header: %.c
	awk -f GenerateApplicationHeader.awk "$<" >"$@"

include ${MK_DIR}/rules.mk
