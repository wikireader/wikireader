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
WIKILIB_COBJS= file-io-libfat.o file-io.o wikilib.o \
                malloc-simple.o search-test.o
vpath %.c $(GUILIBDIR):$(WIKILIBDIR)/generic:$(WIKILIBDIR)/specific

LDFLAGS=-static --strip-all -s --no-gc-sections -N -L$(LIBDIR)
CFLAGS=-Wall -I. -gstabs -mlong-calls -fno-builtin -Os -mc33pe $(INCLUDES)
ASFLAGS=-c -xassembler-with-cpp -Wa,--gstabs -medda32 -mc33pe -mc33_ext

OBJECTS=mahatma.o gui.o serial.o traps.o $(GUILIB_COBJS) $(WIKILIB_COBJS)
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

