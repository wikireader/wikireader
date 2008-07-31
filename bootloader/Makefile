CROSS=c33-epson-elf-
GCC=$(CROSS)gcc
CC=$(GCC)
#LDFLAGS=-static -s -Wl,'-e 0' -Wl,'-z origin' -Wl,'-Ttext=0' -r --gc-sections -N -Ttext=0
LDFLAGS=-static -s -r --no-gc-sections -Wl,'-Ttext=0' 
CFLAGS=-Wall -I. -Iefsl/ -Iefsl/inc/
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

#### e07load
e07load: e07load.c
	gcc -o $@ -Wall $<

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
eeprom-2nd.elf: eeprom-2nd.o $(EFSL_OBJS)
	$(GCC) -o $@ $(LDFLAGS) $^

eeprom-2nd: eeprom-2nd.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --only-section=.text --only-section=.rodata \
				--change-section-address .rodata=0x5000 	\
				--set-start=0x10000000 $< $@

clean::
	rm -fr $(TARGETS) $(EFSL_OBJS) *.o *.elf

