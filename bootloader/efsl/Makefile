CROSS=c33-epson-elf-
GCC=$(CROSS)gcc
CC=$(GCC)
LDFLAGS=-static -s -Wl,'-e 0' -Wl,'-z origin' -Wl,'-Ttext=0' -r --gc-sections -N -Ttext=0
CFLAGS=-I. -I.. -Iinc/

OBJ=src/efs.o src/plibc.o src/disc.o src/partition.o src/time.o src/fs.o src/fat.o src/file.o src/dir.o src/mkfs.o src/debug.o src/ioman.o src/ui.o src/extract.o src/interfaces/s1c33e07_spi.o src/interfaces/sd.o

all: lib

lib: $(OBJ)
	$(AR) rcs libefsl.a $(OBJ)
	
clean :
	rm -f $(OBJ) libefsl.a

