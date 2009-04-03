# Makefile

export PATH:=../install/bin:${PATH}

CROSS = c33-epson-elf-
CROSS_GCC = ${CROSS}gcc
CROSS_AS = ${CROSS}as
CROSS_LD = ${CROSS}ld

RM = rm -f

CROSS_LDFLAGS = -static --strip-all --no-gc-sections --omagic
CROSS_ASFLAGS = -mc33p3 -mc33adv --fatal-warnings

all: forth.elf


FORTH_OBJECTS = forth.o sio.o
forth.elf: ${FORTH_OBJECTS} forth.lds
	${CROSS_LD} ${CROSS_LDFLAGS} -T ${@:.elf=.lds} -Map ${@:.elf=.map} -o $@ ${FORTH_OBJECTS} ${LIBS}


clean:
	${RM} *~ *.o *.lst *.elf *.map

%.o: %.s
	${CROSS_AS} -o $@ ${CROSS_ASFLAGS} -ahlsm=${@:.o=.lst} $<
