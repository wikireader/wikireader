CROSS=c33-epson-elf-
GCC=$(CROSS)gcc
AS=$(CROSS)as
LD=$(CROSS)ld
HOSTCC=gcc
CC=$(GCC)
LIBDIR= /home/daniel/src/axlinux-c33pe.061027/tools/c33/lib/gcc-lib/c33-epson-elf/3.3.2/33401/

#LDFLAGS=-static -s -Wl,'-e 0' -Wl,'-z origin' -Wl,'-Ttext=0' --gc-sections -N -Ttext=0
LDFLAGS=-static -s --no-gc-sections -N \
	-L$(LIBDIR) \

LIBS= 	$(LIBDIR)/libgcc.a $(LIBDIR)/libc.a

CFLAGS=-Wall -I. -Iefsl/ -Iefsl/inc/ -gstabs -mlong-calls -fno-builtin -Os #-medda32 -mc33adv

ASFLAGS=-c -xassembler-with-cpp -Wa,--gstabs -medda32 -mc33pe -mc33_ext

OBJCOPY=$(CROSS)objcopy
OBJCOPY_FLAGS=-I elf32-c33 -O binary 

EFSL_OBJS= \
	efsl/src/efs.o efsl/src/plibc.o efsl/src/disc.o efsl/src/partition.o		\
	efsl/src/time.o efsl/src/fs.o efsl/src/fat.o efsl/src/file.o efsl/src/dir.o	\
	efsl/src/mkfs.o efsl/src/debug.o efsl/src/ioman.o efsl/src/ui.o 		\
	efsl/src/extract.o efsl/src/interfaces/s1c33e07_spi.o 				\
	efsl/src/interfaces/sd.o



TARGETS=rs232 eeprom-1st eeprom-2nd e07load
all: $(TARGETS)

.c.o:
	$(GCC) -M $(CFLAGS) $< > $(<:.c=.d)
	$(GCC) $(CFLAGS) -c $< -o $(<:.c=.o)

#### e07load
e07load: e07load.c
	$(HOSTCC) -o $@ -Wall $^

#### first boot loader
rs232.elf: rs232.o
	$(GCC) -o $@ $(LDFLAGS) $< -Wl,'-Ttext=0'

rs232: rs232.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text --set-start=0 $< $@

#### second boot loader
eeprom-1st.o: eeprom-1st.c eeprom-2nd
	$(GCC) -c $(CFLAGS) -DEEPROM_PAYLOAD_SIZE=`wc -c eeprom-2nd | cut -f1 -d' '` $<

eeprom-1st.elf: eeprom-1st.o
	$(GCC) -o $@ $(LDFLAGS) $< -Wl,'-Ttext=0'

eeprom-1st: eeprom-1st.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text --set-start=0 $< $@

#### third boot loader
eeprom-2nd.elf: eeprom-2nd.o misc.o spi.o sdcard.o crc.o
	$(LD) -o $@ $(LDFLAGS) $^ $(LIBS) -T $(<:.o=.lds) -Map $(<:.o=.map)

eeprom-2nd: eeprom-2nd.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text --only-section=.rodata \
				--pad-to 30000 					\
				$< $@

#--change-section-address .rodata=0x5000
#--set-start=0x10000000 --pad-to 30000

clean::
	rm -fr $(TARGETS) $(EFSL_OBJS) *.o *.elf *.d efsl/src/*.d *.map

-include $(wildcard *.d) dummy

