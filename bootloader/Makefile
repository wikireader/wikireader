CROSS=c33-epson-elf-
GCC=$(CROSS)gcc
CC=$(GCC)
LDFLAGS=-static -s -Wl,'-e 0' -Wl,'-z origin' -Wl,'-Ttext=0' -r --gc-sections -N -Ttext=0
CFLAGS=-Wall -I. -Iefsl/ -Iefsl/inc/
OBJCOPY=$(CROSS)objcopy
OBJCOPY_FLAGS=-I elf32-c33 -O binary 

EFSL_OBJS= \
	efsl/src/efs.o efsl/src/plibc.o efsl/src/disc.o efsl/src/partition.o		\
	efsl/src/time.o efsl/src/fs.o efsl/src/fat.o efsl/src/file.o efsl/src/dir.o	\
	efsl/src/mkfs.o efsl/src/debug.o efsl/src/ioman.o efsl/src/ui.o 		\
	efsl/src/extract.o efsl/src/interfaces/s1c33e07_spi.o 				\
	efsl/src/interfaces/sd.o


TARGETS=rs232 eeprom.img e07load
all: $(TARGETS)

e07load: e07load.c
	gcc -o $@ -Wall $<

rs232.elf: rs232.o
	$(GCC) -o $@ $(LDFLAGS) $<

eeprom-1st.elf: eeprom-1st.o
	$(GCC) -o $@ $(LDFLAGS) $<

eeprom-2nd.elf: eeprom-2nd.o $(EFSL_OBJS)
	$(GCC) -o $@ $(LDFLAGS) $^

rs232: rs232.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --set-start=0 $< $@
	@ls -l $@
	$(OBJCOPY) $(OBJCOPY_FLAGS) --set-start=0 --pad-to 512 $< $@

eeprom-1st: eeprom-1st.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --set-start=0 --pad-to 512 $< $@

eeprom-2nd: eeprom-2nd.elf
	$(OBJCOPY) $(OBJCOPY_FLAGS) --set-start=512 $< $@

eeprom.img: eeprom-2nd eeprom-1st
	@dd if=eeprom-1st of=$@ bs=512 count=1
	@dd if=eeprom-2nd of=$@ bs=512 seek=1

clean::
	rm -fr $(TARGETS) $(EFSL_OBJS) *.o *.elf


