# Copyright (c) 2009 Openmoko Inc.
#
# Authors   Christopher Hall <hsw@openmoko.com>
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

build:
	${MKDIR} "$@"

lib:
	${MKDIR} "$@"


.PHONY: install
install: all
	@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	${COPY} flash.elf "${DESTDIR}"

.PHONY: clean
clean:
	${RM} -r ${TARGETS} build lib
	${RM} -r *.o *.elf *.d *.map *.asm33

include ${MK_DIR}/rules.mk
