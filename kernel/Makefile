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

CROSS=c33-epson-elf-
GCC=$(CROSS)gcc
AS=$(CROSS)as
LD=$(CROSS)ld
HOSTCC=gcc
CC=$(GCC)

WIKILIBDIR=$(PWD)/../wiki-lib/
GUILIBDIR=$(PWD)/../gui-lib/
LIBDIR=$(PWD)/../toolchain/mini-libc
FATFSDIR=$(PWD)/../fatfs

INCLUDES=				\
	-I$(FATFSDIR)/src		\
	-I$(LIBDIR)/include/		\
	-I$(FATFSDIR)/config/c33/	\
	-I$(WIKILIBDIR)/include/	\
	-I$(GUILIBDIR)/			\
	-I../bootloader/

LIBS=					\
	$(FATFSDIR)/lib/libtinyfat.a	\
	$(LIBDIR)/lib/libc.a 		\
	`$(CC) -print-libgcc-file-name`

GUILIB_COBJS= guilib.o glyph.o fontfile.o
WIKILIB_COBJS= file-io-libfat.o wikilib.o \
                malloc-simple.o 
vpath %.c $(GUILIBDIR):$(WIKILIBDIR)/generic:$(WIKILIBDIR)/specific

LDFLAGS=-static --strip-all -s --no-gc-sections -N -L$(LIBDIR)
CFLAGS=-Wall -I. -gstabs -mlong-calls -fno-builtin -Os -mc33pe $(INCLUDES)
ASFLAGS=-c -xassembler-with-cpp -Wa,--gstabs -medda32 -mc33pe -mc33_ext

OBJECTS=mahatma.o touchscreen.o suspend.o msg-output.o gui.o	\
	serial.o traps.o $(GUILIB_COBJS) $(WIKILIB_COBJS)
TARGETS=mahatma.elf
all: $(TARGETS)

#.c.o:
#	$(GCC) -M $(CFLAGS) $< > $(<:.c=.d)
#	$(GCC) $(CFLAGS) -c $< -o $(<:.c=.o)

mahatma.elf: $(OBJECTS) mahatma.lds
	$(LD) -o $@ $(LDFLAGS) $^ $(LIBS) -T $(<:.o=.lds) -Map $(<:.o=.map)

clean::
	rm -fr $(TARGETS) *.o *.elf *.d *.map

-include $(wildcard *.d) dummy

