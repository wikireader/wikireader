# Copyright (c) 2009 Christopher Hall <hsw@openmoko.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


SAMO_LIB := $(shell readlink -es ../samo-lib || readlink -es ../../samo-lib || readlink -es ../../../samo-lib)
FATFS_MODE = read-write

include ${SAMO_LIB}/Mk/definitions.mk

INCLUDES += -I$(MINI_LIBC_INCLUDE)
INCLUDES += -I${DRIVERS_INCLUDE}
INCLUDES += -I${FATFS_CONFIG_INCLUDE} -I${FATFS_INCLUDE}
INCLUDES += -I${SAMO_LIB_INCLUDE}
INCLUDES += -I$(INDEXING_INCLUDE)
INCLUDES += -I$(MINILZO_INCLUDE)
INCLUDES += -I$(WIKI_APP_INCLUDE)

LIBS += ${FATFS_LIB}
LIBS += ${DRIVERS_LIB}
LIBS += ${MINI_LIBC_LIB}
LIBS += $(shell $(CC) -print-libgcc-file-name)

BUILD_PREFIX := build/

vpath %.c $(WIKI_APP_SRC):$(INDEXING_SRC):$(MINILZO_SRC)


OBJECTS = flash.o
OBJECTS += misc.o
OBJECTS += $(GUILIB_COBJS) $(WIKIAPP_COBJS) $(MINILZO_COBJS) $(PERF_COBJ)


BUILD_OBJECTS = $(addprefix ${BUILD_PREFIX},${OBJECTS})

TARGETS = ok.h fail.h program.h flash.elf

.PHONY:	all
all: $(TARGETS)

lib/libflash.a: lib build ${BUILD_OBJECTS}
	${AR} r "$@" ${BUILD_OBJECTS}

flash.elf: build build/flash.o lib/libflash.a flash.lds ${LIBS}
	$(LD) -o $@ ${LDFLAGS} build/flash.o lib/libflash.a ${LIBS} -T ${@:.elf=.lds} -Map ${@:.elf=.map}


ok.h: ok.png
fail.h: fail.png
program.h: program.png

%.h: %.png
	$(HOST_TOOLS)/imagetool/imagetool.py "$<" "$@" "${@:.h=_image}"


build:
	${MKDIR} "$@"

lib:
	${MKDIR} "$@"

clean:
	${RM} -r ${TARGETS} build lib
	${RM} -r *.o *.elf *.d *.map *.asm33

include ${SAMO_LIB}/Mk/rules.mk
