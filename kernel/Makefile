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
#ifndef SIMULATOR 
PROFILER_ON = -DPROFILER_ON
#ednif

WIKILIBDIR=../wiki-lib/
GUILIBDIR=../gui-lib/
LIBDIR=../toolchain/mini-libc
FATFSDIR=../fatfs
FATFS_MODE = read-write
INDEX_DIR=../host-tools/indexing/locate
COMMON=../common
MINILZODIR=../minilzo

INCLUDES = -I$(FATFSDIR)/src
INCLUDES += -I$(LIBDIR)/include/
INCLUDES += -I$(FATFSDIR)/config/c33/${FATFS_MODE}/
INCLUDES += -I$(WIKILIBDIR)/include/
INCLUDES += -I$(GUILIBDIR)/
INCLUDES += -I../common/
INCLUDES += -I$(INDEX_DIR)
INCLUDES += -I$(MINILZODIR)/

LIBS = $(FATFSDIR)/lib/${FATFS_MODE}/libtinyfat.a
LIBS += $(LIBDIR)/lib/libc.a
LIBS += `$(CC) -print-libgcc-file-name`

GUILIB_COBJS = guilib.o glyph.o fontfile.o

WIKILIB_COBJS = file-io-general.o file-io-libfat.o
WIKILIB_COBJS += wikilib.o malloc-simple.o hexdump.o
WIKILIB_COBJS += decompress.o keyboard.o

# searching
WIKILIB_COBJS += search.o lsearcher.o fast_search.o
WIKILIB_COBJS += article.o history.o wom_reader.o

# minilzo
MINILZO_COBJS = minilzo.o

# convert the keyboard file
PNM2PLAIN_NAME = pnmtoplainpnm
PNM2PLAIN_PATH := $(shell which $(PNM2PLAIN_NAME))
KBD_IMG_SRC_P4 = ../images/keyboard_abc_mono2.pbm
KBD_IMG_SRC_P1 = ../images/keyboard_abc_mono2-plain.pbm
KBD_IMG_INC = $(WIKILIBDIR)include/keyboard_image.h
KBD_CONVERT = ../scripts/kbd_to_include.py

CFLAGS+=-DINCLUDED_FROM_KERNEL $(PROFILER_ON)
LDFLAGS=-static --strip-all -s --no-gc-sections -N -L$(LIBDIR)
##################################LDFLAGS=-static --strip-all --no-gc-sections --omagic -L$(LIBDIR)

vpath %.c $(GUILIBDIR):$(WIKILIBDIR)/generic:$(WIKILIBDIR)/specific:$(INDEX_DIR):$(COMMON):$(MINILZODIR)

OBJECTS = mahatma.o touchscreen.o suspend.o msg-output.o gui.o tick.o
OBJECTS += gpio.o serial.o traps.o input.o lcd.o misc.o profile.o
OBJECTS += $(GUILIB_COBJS) $(WIKILIB_COBJS) $(MINILZO_COBJS)

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
	$(LD) -o $@ $(LDFLAGS) $^ $(LIBS) -T $(<:.o=.lds) -Map $(<:.o=.map)

clean::
	rm -fr $(TARGETS) *.o *.elf *.d *.map *.asm33 $(KBD_IMG_INC) $(KBD_IMG_SRC_P1)

include ../common/Makefile
