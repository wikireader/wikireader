# Makefile
#
# Copyright 2009 Christopher Hall <hsw@openmoko.com>
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


INCLUDES += -I.
INCLUDES += -I${DRIVERS_INCLUDE}
INCLUDES += -I${SAMO_LIB_INCLUDE}
INCLUDES += -I${FATFS_CONFIG_INCLUDE} -I${FATFS_INCLUDE}
INCLUDES += -I${MINI_LIBC_INCLUDE}

LIBS += ${FATFS_LIB}
LIBS += ${DRIVERS_LIB}
LIBS += ${MINI_LIBC_LIB}
LIBS += $(shell ${CROSS_GCC} -print-libgcc-file-name)

CROSS_LDFLAGS = -static --strip-all --no-gc-sections --omagic
CROSS_ASFLAGS = -mc33pe --fatal-warnings
CLEAN_FILES =

TARGETS = forth.elf calc.elf
FORTH_PROGRAMS = programs/*.4?? programs/*.ini programs/*.tst

.PHONY: all
all: ${TARGETS}


OBJECTS =
OBJECTS += button.o
OBJECTS += ctp.o
OBJECTS += debug.o
OBJECTS += FileSystem.o
OBJECTS += iserial.o
OBJECTS += PolledSerial.o
OBJECTS += vector.o

CLEAN_FILES += forth.s
forth.s: header.s meta.fs forth-symbols.fi ansi-forth.fs forth-vector.fi trailer.s
	${RM} "$@"
	cat header.s >> "$@"
	gforth meta.fs forth-symbols.fi ansi-forth.fs forth-vector.fi -e "bye" >> "$@"
	cat trailer.s  >> "$@"

forth-symbols.fi : sym.awk ansi-forth.fs forth-vector.fi
	awk -f sym.awk ansi-forth.fs forth-vector.fi > "$@"

forth-vector.fi: Makefile
	${RM} "$@"
	echo "\\ generated file - do not modify" >> "$@"
	echo >> "$@"
	echo "only forth definitions" >> "$@"
	echo "also meta-compiler" >> "$@"
	echo "meta-compile" >> "$@"
	echo >> "$@"
	echo "variable 'cold-run :: tick-cold-run" >> "$@"


CLEAN_FILES += calc.s
calc.s: header.s meta.fs calc-symbols.fi ansi-forth.fs calc.fi trailer.s
	${RM} "$@"
	cat header.s >> "$@"
	gforth meta.fs calc-symbols.fi ansi-forth.fs calc.fi -e "bye" >> "$@"
	cat trailer.s  >> "$@"

calc-symbols.fi : sym.awk ansi-forth.fs calc.fi
	awk -f sym.awk ansi-forth.fs calc.fi > "$@"

calc.fi: programs/calc.4th
	${RM} "$@"
	echo "\\ generated file - do not modify" >> "$@"
	echo >> "$@"
	echo "only forth definitions" >> "$@"
	echo "also meta-compiler" >> "$@"
	echo "meta-compile" >> "$@"
	echo >> "$@"
	cat $^ >> "$@"
	echo >> "$@"
	echo "create  'cold-run :: tick-cold-run" >> "$@"
	echo "<',> calculator" >> "$@"


.PHONY: install
install: all
	@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	${COPY} ${TARGETS} "${DESTDIR}"
	${COPY} ${FORTH_PROGRAMS} "${DESTDIR}"


.PHONY: clean
clean:
	${RM} *~ *.o *.d *.lst *.elf *.map *.asm33 *.fi
	${RM} ${CLEAN_FILES}

%.o: %.s
	${CROSS_AS} -o $@ ${CROSS_ASFLAGS} -ahlsm=${@:.o=.lst} $<


%.elf: %.o ${OBJECTS} %.lds ${LIBS}
	${CROSS_LD} ${CROSS_LDFLAGS} -T "${@:.elf=.lds}" -Map "${@:.elf=.map}" -o "$@" "$<" ${OBJECTS} ${LIBS}
	${CROSS_STRIP} --remove-section=.comment "$@"
	${CROSS_OBJDUMP} --section-headers "$@"


include ${SAMO_LIB}/Mk/rules.mk
