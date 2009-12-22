# Makefile
#
# Copyright (c) 2009 Openmoko Inc.
#
# Authors   Christopher Hall <hsw@openmoko.com>
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

FATFS_MODE = read-write

# +++START_UPDATE_MAKEFILE: Start of auto included code
# The text between the +++ and --- tags is copied by the
# UpdateMakefiles script. Do not remove or change these tags.
# ---
# Autodetect root directory
define FIND_ROOT_DIR
while : ; do \
  d=$$(pwd) ; \
  [ -d "$${d}/samo-lib" ] && echo $${d} && exit 0 ; \
  [ X"/" = X"$${d}" ] && echo ROOT_DIRECTORY_NOT_FOUND && exit 1 ; \
  cd .. ; \
done
endef
ROOT_DIR := $(shell ${FIND_ROOT_DIR})
# Directory of Makefile includes
MK_DIR   := ${ROOT_DIR}/samo-lib/Mk
# Include the initial Makefile setup
include ${MK_DIR}/definitions.mk
# ---END_UPDATE_MAKEFILE: End of auto included code

$(call REQUIRED_BINARY, ${AWK}, awk)
$(call REQUIRED_BINARY, gforth, gforth)


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

TARGETS = forth.elf

FORTH_PROGRAMS = programs/*.4?? programs/*.ini programs/*.tst

.PHONY: all
all: ${TARGETS} applications


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
	${AWK} -f sym.awk ansi-forth.fs forth-vector.fi > "$@"

forth-vector.fi: Makefile
	${RM} "$@"
	echo "\\ generated file - do not modify" >> "$@"
	echo >> "$@"
	echo "only forth definitions" >> "$@"
	echo "also meta-compiler" >> "$@"
	echo "meta-compile" >> "$@"
	echo >> "$@"
	echo "variable 'cold-run :: tick-cold-run" >> "$@"

forth.elf: forth.o ${OBJECTS} ${LIBS}


# Common macro for compiling individual forth applications
MAKE_FORTH_APP = $(eval $(call MAKE_FORTH_APP1,$(strip ${1}),$(strip ${2}),$(strip ${3})))

# the main application build rules
define MAKE_FORTH_APP1
CLEAN_FILES += ${1}.s
CLEAN_FILES += ${1}.lds
APPLICATIONS += ${1}.elf
${1}.s: header.s meta.fs ${1}-symbols.fi ansi-forth.fs ${1}.fi trailer.s
	$${RM} "$$@"
	cat header.s >> "$$@"
	gforth meta.fs ${1}-symbols.fi ansi-forth.fs ${1}.fi -e "bye" >> "$$@"
	cat trailer.s  >> "$$@"

${1}-symbols.fi : sym.awk ansi-forth.fs ${1}.fi
	${AWK} -f sym.awk ansi-forth.fs ${1}.fi > "$$@"

${1}.lds: forth.lds
	sed 's/forth\.o/${1}.o/g' <forth.lds > "$$@"

${1}.fi: $(wildcard programs/${1}.4th) $(wildcard programs/${1}.forth)
	$${RM} "$$@"
	echo "\\ generated file - do not modify" >> "$$@"
	echo >> "$$@"
	echo "only forth definitions" >> "$$@"
	echo "also meta-compiler" >> "$$@"
	echo "meta-compile" >> "$$@"
	echo >> "$$@"
	cat $$^ >> "$$@"
	echo >> "$$@"
	echo "create  'cold-run :: tick-cold-run" >> "$$@"
	echo "<',> ${2}" >> "$$@"

${1}.elf: ${1}.o ${OBJECTS} ${LIBS}

endef

# list of individual applications to cross compile

$(call MAKE_FORTH_APP,calc,calculator)
$(call MAKE_FORTH_APP,mtbf,mtbf-main)
$(call MAKE_FORTH_APP,cob,cob-main)


# target for compiling all appications
.PHONY: applications
applications: ${APPLICATIONS}


.PHONY: install
install: all
	@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	${COPY} ${TARGETS} ${APPLICATIONS} "${DESTDIR}"
	${COPY} ${FORTH_PROGRAMS} "${DESTDIR}"


.PHONY: clean
clean:
	${RM} *~ *.o *.d *.lst *.elf *.map *.asm33 *.fi *.p
	${RM} ${CLEAN_FILES}

%.o: %.s
	${CROSS_AS} -o $@ ${CROSS_ASFLAGS} -ahlsm=${@:.o=.lst} $<


%.elf: %.o ${OBJECTS} %.lds ${LIBS}
	${CROSS_LD} ${CROSS_LDFLAGS} -T "${@:.elf=.lds}" -Map "${@:.elf=.map}" -o "$@" "$<" ${OBJECTS} ${LIBS}
	${CROSS_STRIP} --remove-section=.comment "$@"
	${CROSS_OBJDUMP} --section-headers "$@"


include ${MK_DIR}/rules.mk
