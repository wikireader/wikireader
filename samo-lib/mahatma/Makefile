# Copyright (c) 2009 Openmoko Inc.
#
# Authors   Daniel Mack <daniel@caiaq.de>
#           Holger Hans Peter Freyther <zecke@openmoko.org>
#           Christopher Hall <hsw@openmoko.com>
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

# (de)activate profiling
#PROFILER_ON = -DPROFILER_ON

FATFS_MODE = read-write


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

INCLUDES += -I$(MINI_LIBC_INCLUDE)
INCLUDES += -I${DRIVERS_INCLUDE}
INCLUDES += -I${FATFS_CONFIG_INCLUDE} -I${FATFS_INCLUDE}
INCLUDES += -I${SAMO_LIB_INCLUDE}
INCLUDES += -I$(LZMA_INCLUDE)
INCLUDES += -I$(WIKI_APP_INCLUDE)

LIBS += ${FATFS_LIB}
LIBS += ${DRIVERS_LIB}
LIBS += ${MINI_LIBC_LIB}
LIBS += $(shell $(CC) -print-libgcc-file-name)

BUILD_PREFIX := build/
INCLUDES += -I$(BUILD_PREFIX)

vpath %.c $(WIKI_APP_SRC):$(LZMA_SRC)
vpath %.png $(WIKI_APP_SRC)


GUILIB_COBJS = guilib.o
GUILIB_COBJS += bmf.o
GUILIB_COBJS += lcd_buf_draw.o
GUILIB_COBJS += bigram.o
GUILIB_COBJS += glyph.o
GUILIB_COBJS += restricted.o
GUILIB_COBJS += sha1.o
GUILIB_COBJS += wiki_info.o

WIKIAPP_COBJS = file-io-libfat.o
WIKIAPP_COBJS += wikilib.o
WIKIAPP_COBJS += malloc-simple.o
WIKIAPP_COBJS += keyboard.o
WIKIAPP_COBJS += utf8.o

# used for panic in traps.c
WIKIAPP_COBJS += hexdump.o
WIKIAPP_COBJS += misc.o

# searching
WIKIAPP_COBJS += search.o
WIKIAPP_COBJS += search_hash.o
WIKIAPP_COBJS += history.o

# lzma
LZMA_COBJS = LzmaDec.o
LZMA_COBJS += Alloc.o
LZMA_COBJS += Bra.o
LZMA_COBJS += LzFind.o

# convert the image files
IMAGES = keyboard_abc keyboard_abc2 keyboard_123 keyboard_phone_jp keyboard_phone_abc keyboard_phone_123 nls

CFLAGS+=-DINCLUDED_FROM_KERNEL $(PROFILER_ON)
LDFLAGS=-static --strip-all -s --no-gc-sections -N -L$(MINI_LIBC)


OBJECTS = mahatma.o
OBJECTS += touchscreen.o
OBJECTS += msg-output.o
OBJECTS += gui.o
OBJECTS += gpio.o
OBJECTS += serial.o
OBJECTS += traps.o
OBJECTS += input.o
OBJECTS += profile.o
OBJECTS += $(GUILIB_COBJS)
OBJECTS += $(WIKIAPP_COBJS)
OBJECTS += $(LZMA_COBJS)

BUILD_OBJECTS = $(addprefix ${BUILD_PREFIX},${OBJECTS})

ALL_IMAGES = $(addprefix ${BUILD_PREFIX},all_images.h)

TARGETS = ${ALL_IMAGES}
TARGETS += mahatma.elf

.PHONY:	all
all: ${TARGETS}


lib/libmahatma.a: lib build ${BUILD_OBJECTS}
	${AR} r "$@" ${BUILD_OBJECTS}

mahatma.elf: build build/mahatma.o lib/libmahatma.a mahatma.lds ${LIBS}
	$(LD) -o $@ ${LDFLAGS} build/mahatma.o lib/libmahatma.a ${LIBS} -T ${@:.elf=.lds} -Map ${@:.elf=.map}
	${OBJDUMP} --section-headers "$@"
	${OBJDUMP} -D "$@" > "${@:.elf=.dump}"

IMAGE_HEADERS = $(addsuffix .h,$(addprefix ${BUILD_PREFIX},$(strip ${IMAGES})))
CLEAN_FILES += ${IMAGE_HEADERS}
${ALL_IMAGES}: build ${IMAGE_HEADERS}
	@${RM} "$@"
	@for f in ${IMAGE_HEADERS}; \
	 do \
	   echo Keyboard: $$(basename "$${f}") ; \
	   echo '#include "'$$(basename "$${f}")'"' >> "$@"; \
	 done


build:
	${MKDIR} "$@"

lib:
	${MKDIR} "$@"

.PHONY: install
install: all
	@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	${COPY} mahatma.elf "${DESTDIR}"/kernel.elf

.PHONY: clean
clean:
	${RM} ${TARGETS}
	${RM} -r  build lib
	${RM} *.o *.elf *.d *.map *.asm33 *.dump
	${RM} ${CLEAN_FILES}

include ${MK_DIR}/rules.mk
