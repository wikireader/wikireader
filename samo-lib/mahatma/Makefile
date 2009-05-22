# mahatma - a simple kernel framework
# Copyright (c) 2008, 2009 Daniel Mack <daniel@caiaq.de>
# Copyright (c) 2008 Holger Hans Peter Freyther <zecke@openmoko.org>
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
PROFILER_ON = -DPROFILER_ON


SAMO_LIB := $(shell readlink -es ../samo-lib || readlink -es ../../samo-lib || readlink -es ../../../samo-lib)
FATFS_MODE = read-write

include ${SAMO_LIB}/Mk/definitions.mk

INCLUDES += -I$(MINI_LIBC_INCLUDE)
INCLUDES += -I${DRIVERS_INCLUDE}
INCLUDES += -I${FATFS_CONFIG_INCLUDE} -I${FATFS_INCLUDE}
INCLUDES += -I${SAMO_LIB_INCLUDE}
INCLUDES += -I$(INDEXING_INCLUDE)
INCLUDES += -I$(MINILZO_INCLUDE)
INCLUDES += -I$(WIKI_APP_INCLUDE)

LIBS += ${FATFS_LIB}
LIBS += ${DRIVERS_LIB}
LIBS += ${MINI_LIBC_LIB}
LIBS += `$(CC) -print-libgcc-file-name`

BUILD_PREFIX := build/

vpath %.c $(WIKI_APP_SRC):$(INDEXING_SRC):$(SAMO_LIB_SRC):$(MINILZO_SRC)


GUILIB_COBJS = guilib.o glyph.o fontfile.o

WIKIAPP_COBJS = file-io-general.o file-io-libfat.o
WIKIAPP_COBJS += wikilib.o malloc-simple.o hexdump.o
WIKIAPP_COBJS += decompress.o keyboard.o random.o

# searching
WIKIAPP_COBJS += search.o lsearcher.o fast_search.o
WIKIAPP_COBJS += article.o history.o wom_reader.o

# minilzo
MINILZO_COBJS = minilzo.o

# performance
# PERF_COBJ = perf.o

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
OBJECTS += touchscreen.o suspend.o msg-output.o gui.o tick.o
OBJECTS += gpio.o serial.o traps.o input.o lcd.o misc.o profile.o
OBJECTS += $(GUILIB_COBJS) $(WIKIAPP_COBJS) $(MINILZO_COBJS) $(PERF_COBJ)

ifeq ($(PNM2PLAIN_PATH),)
$(error Could not find $(PNM2PLAIN_NAME) - please install netpbm)
endif

TARGETS=$(KBD_IMG_INC) mahatma.elf
.PHONY:	all
all: $(TARGETS)

$(KBD_IMG_SRC_P1): $(KBD_IMG_SRC_P4)
	$(PNM2PLAIN_PATH) $(KBD_IMG_SRC_P4) > $@

$(KBD_IMG_INC): $(KBD_CONVERT) $(KBD_IMG_SRC_P1)
	$(KBD_CONVERT) $(KBD_IMG_SRC_P1) > $@

mahatma.elf: $(OBJECTS) mahatma.lds
	$(LD) -o $@ $(LDFLAGS) ${OBJECTS} $(LIBS) -T $(<:.o=.lds) -Map $(<:.o=.map)

clean::
	rm -fr $(TARGETS) *.o *.elf *.d *.map *.asm33 $(KBD_IMG_INC) $(KBD_IMG_SRC_P1)


include ${SAMO_LIB}/Mk/rules.mk
