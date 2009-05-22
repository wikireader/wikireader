# Makefile

#	Copyright 2009 Christopher Hall <hsw@openmoko.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

SAMO_LIB := $(shell readlink -es ../samo-lib || readlink -es ../../samo-lib || readlink -es ../../../samo-lib)
FATFS_MODE = read-write

include ${SAMO_LIB}/Mk/definitions.mk

export PATH:=../../host-tools/toolchain-install/bin:${PATH}

INCLUDES += -I${SAMO_LIB_INCLUDE}
INCLUDES += -I${WIKI_APP_INCLUDE}
INCLUDES += -I${DRIVERS_INCLUDE}
INCLUDES += -I${FATFS_CONFIG_INCLUDE} -I${FATFS_INCLUDE}
INCLUDES += -I${MINI_LIBC_INCLUDE}


LIBS +=	${FATFS_LIB}
LIBS +=	${DRIVERS_LIB}
LIBS +=	${MINI_LIBC_LIB}
LIBS += $$(${CROSS_GCC} -print-libgcc-file-name)

RM = rm -f

CROSS_LDFLAGS = -static --strip-all --no-gc-sections --omagic
CROSS_ASFLAGS = -mc33pe --fatal-warnings


#vpath %.c src


all: forth.elf


FORTH_OBJECTS = forth.o serial.o debug.o FileSystem.o
forth.elf: ${FORTH_OBJECTS} forth.lds
	${CROSS_LD} ${CROSS_LDFLAGS} -T ${@:.elf=.lds} -Map ${@:.elf=.map} -o $@ ${FORTH_OBJECTS} ${LIBS}
	${CROSS_STRIP} --remove-section=.comment $@
	${CROSS_OBJDUMP} --section-headers $@


clean:
	${RM} *~ *.o *.d *.lst *.elf *.map *.asm33

%.o: %.s
	${CROSS_AS} -o $@ ${CROSS_ASFLAGS} -ahlsm=${@:.o=.lst} $<

include ${SAMO_LIB}/Mk/rules.mk
