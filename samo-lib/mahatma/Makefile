# mahatma - a simple kernel framework
# Copyright (c) 2008, 2009 Daniel Mack <daniel@caiaq.de>
# Copyright (c) 2008 Holger Hans Peter Freyther <zecke@openmoko.org>
# Copyright (c) 2009 Openmoko Inc.
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


SAMO_LIB := $(shell readlink -es ../samo-lib || readlink -es ../../samo-lib || readlink -es ../../../samo-lib)
FATFS_MODE = read-write

include ${SAMO_LIB}/Mk/definitions.mk

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

vpath %.c $(WIKI_APP_SRC):$(LZMA_SRC)


GUILIB_COBJS = guilib.o
GUILIB_COBJS += bmf.o
GUILIB_COBJS += lcd_buf_draw.o
GUILIB_COBJS += bigram.o
GUILIB_COBJS += glyph.o
GUILIB_COBJS += restricted.o
GUILIB_COBJS += sha1.o

WIKIAPP_COBJS = file-io-libfat.o
WIKIAPP_COBJS += wikilib.o
WIKIAPP_COBJS += malloc-simple.o
WIKIAPP_COBJS += keyboard.o

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

# convert the keyboard file
PNM2PLAIN_NAME = pnmtoplainpnm
PNM2PLAIN_PATH := $(shell which $(PNM2PLAIN_NAME))
KBD_IMG_SRC_P4 = $(WIKI_APP)/keyboard_abc_mono2.pbm
KBD_IMG_SRC_P1 = $(WIKI_APP)/keyboard_abc_mono2-plain.pbm
KBD_IMG_INC = $(WIKI_APP)/keyboard_image.h
KBD_CONVERT = ../../wiki-app/kbd_to_include.py

CFLAGS+=-DINCLUDED_FROM_KERNEL $(PROFILER_ON)
LDFLAGS=-static --strip-all -s --no-gc-sections -N -L$(MINI_LIBC)
##################################LDFLAGS=-static --strip-all --no-gc-sections --omagic -L$(MINI_LIBC)


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

ifeq ($(PNM2PLAIN_PATH),)
$(error Could not find $(PNM2PLAIN_NAME) - please: sudo apt-get install netpbm)
endif

BUILD_OBJECTS = $(addprefix ${BUILD_PREFIX},${OBJECTS})

TARGETS=$(KBD_IMG_INC) mahatma.elf

.PHONY:	all
all: $(TARGETS)

$(KBD_IMG_SRC_P1): $(KBD_IMG_SRC_P4)
	$(PNM2PLAIN_PATH) $(KBD_IMG_SRC_P4) > $@

$(KBD_IMG_INC): $(KBD_CONVERT) $(KBD_IMG_SRC_P1)
	$(KBD_CONVERT) $(KBD_IMG_SRC_P1) > $@

lib/libmahatma.a: lib build ${BUILD_OBJECTS}
	${AR} r "$@" ${BUILD_OBJECTS}

mahatma.elf: build build/mahatma.o lib/libmahatma.a mahatma.lds ${LIBS}
	$(LD) -o $@ ${LDFLAGS} build/mahatma.o lib/libmahatma.a ${LIBS} -T ${@:.elf=.lds} -Map ${@:.elf=.map}

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
	${RM} -r ${TARGETS} build lib
	${RM} -r *.o *.elf *.d *.map *.asm33 ${KBD_IMG_INC} ${KBD_IMG_SRC_P1}

include ${SAMO_LIB}/Mk/rules.mk
