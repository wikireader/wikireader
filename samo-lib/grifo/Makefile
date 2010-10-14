# Makefile
#
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


INSTALL_GRIFO_SIMULATION ?= NO

$(call REQUIRED_BINARY, guile, guile-1.8)

INCLUDES += -Isrc
INCLUDES += -I-
INCLUDES += -I${SAMO_LIB_INCLUDE}
INCLUDES += -I${FATFS_INCLUDE} -I${FATFS_CONFIG_INCLUDE}
INCLUDES += -I${DRIVERS_INCLUDE}
INCLUDES += -I${MINI_LIBC_INCLUDE}
INCLUDES += -Ibuild -Icommon

LIBS += build/libinternal.a
LIBS += ${FATFS_LIB}
LIBS += ${MINI_LIBC_LIB}
LIBS += ${DRIVERS_LIB}
LIBS += $(shell $(CC) -print-libgcc-file-name)

BUILD_PREFIX := build/

MAKE_SYSCALL := scripts/MakeSystemCalls

vpath %.c :src:common

TARGETS = build/syscall.table
TARGETS += build/libinternal.a
TARGETS += grifo.elf
TARGETS += lib/libgrifo.a

# list of modules for creating libinternal.a
OBJECTS += analog.o
OBJECTS += button.o
OBJECTS += CMU.o
OBJECTS += CTP.o
OBJECTS += delay.o
OBJECTS += elf32.o
OBJECTS += event.o
OBJECTS += file.o
OBJECTS += graphics.o
OBJECTS += LCD.o
OBJECTS += main.o
OBJECTS += memory.o
OBJECTS += serial.o
OBJECTS += suspend.o
OBJECTS += syscall.o
OBJECTS += system.o
OBJECTS += timer.o
OBJECTS += vector.o
OBJECTS += watchdog.o
#OBJECTS +=

BUILD_OBJECTS = $(addprefix ${BUILD_PREFIX},${OBJECTS})

.PHONY: all
all:  ${TARGETS}
	${MAKE} -C examples

build/libinternal.a: stamp-build ${BUILD_OBJECTS}
	${RM} "$@"
	${AR} r "$@" ${BUILD_OBJECTS}

include/grifo.h stubs/stamp-source build/syscall.table: stamp-include stamp-build stamp-stubs src/syscall.list ${MAKE_SYSCALL} src/*.h
	${MAKE_SYSCALL} --prefix=stubs/ \
	  --header=include/grifo.h \
	  --list=src/syscall.list \
	  --table=build/syscall.table \
	  --verbose
	${TOUCH} stubs/stamp-source

stubs/stamp-objects: $(addsuffix .o, $(basename $(wildcard stubs/*.s)))
	${TOUCH} "$@"

stubs/stamp: stamp-stubs
	${MAKE} stubs/stamp-objects
	${TOUCH} "$@"

# this should depend on lib, but make always rebuilds in that case; why?
lib/libgrifo.a: stamp-lib stubs/stamp stubs/stamp-source
	${RM} "$@"
	${AR} r "$@" stubs/*.o
	${TOUCH} "$@"

grifo.elf: build/main.o lds/grifo.lds ${LIBS}
	$(LD) -o $@ ${LDFLAGS} build/main.o ${LIBS} -T lds/${@:.elf=.lds} -Map ${@:.elf=.map}
	${STRIP} --strip-unneeded --strip-debug \
		--remove-section=.internal --remove-section=.lcd \
		--remove-section=.user_area --remove-section=.descriptor "$@"
	${OBJDUMP} --section-headers "$@"
	${OBJDUMP} -D "$@" > "${@:.elf=.dump}"

stamp-build:
	${MKDIR} "${@:stamp-%=%}"
	${TOUCH} "$@"

stamp-lib:
	${MKDIR} "${@:stamp-%=%}"
	${TOUCH} "$@"

stamp-stubs:
	${MKDIR} "${@:stamp-%=%}"
	${TOUCH} "$@"

stamp-include:
	${MKDIR} "${@:stamp-%=%}"
	${TOUCH} "$@"


.PHONY: install
install: all
	@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
ifneq (YES,$(strip ${INSTALL_GRIFO_SIMULATION}))
	${COPY} grifo.elf "${DESTDIR}"/kernel.elf
endif
	${COPY} init.ini "${DESTDIR}"/
	${MAKE} -C examples install INSTALL_GRIFO_SIMULATION="${INSTALL_GRIFO_SIMULATION}"

.PHONY: clean
clean:
	${RM} -r ${TARGETS} build include lib stubs
	${RM} -r *.o *.elf *.d *.map *.asm33 *.dump *.ico stamp-*
	${MAKE} -C examples clean


include ${MK_DIR}/rules.mk
