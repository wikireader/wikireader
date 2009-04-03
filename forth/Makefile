# Makefile

export PATH:=../install/bin:${PATH}

CROSS = c33-epson-elf-
CROSS_GCC = ${CROSS}gcc
CROSS_AS = ${CROSS}as
CROSS_LD = ${CROSS}ld


COMMON = ../common
FATFSDIR = ../fatfs
FATFS_SRC = ${FATFSDIR}/src
LIBDIR=../toolchain/mini-libc

INCLUDES += -I${COMMON}
INCLUDES += -I${FATFS_SRC}
INCLUDES += -I$(LIBDIR)/include

LIBS +=	$(LIBDIR)/lib/libc.a
LIBS += $$($(CROSS_GCC) -print-libgcc-file-name)

RM = rm -f

CROSS_LDFLAGS = -static --strip-all --no-gc-sections --omagic
CROSS_ASFLAGS = -mc33pe --fatal-warnings


vpath %.c :${COMMON}:${FATFS_SRC}


all: forth.elf


FORTH_OBJECTS = forth.o serial.o debug.o mmc.o tff.o FileSystem.o
forth.elf: ${FORTH_OBJECTS} forth.lds
	${CROSS_LD} ${CROSS_LDFLAGS} -T ${@:.elf=.lds} -Map ${@:.elf=.map} -o $@ ${FORTH_OBJECTS} ${LIBS}


clean:
	${RM} *~ *.o *.d *.lst *.elf *.map

%.o: %.s
	${CROSS_AS} -o $@ ${CROSS_ASFLAGS} -ahlsm=${@:.o=.lst} $<



include ../common/Makefile
