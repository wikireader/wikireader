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


define FIND_SAMO_LIB
d='.' ; \
while [ X"/" != X"$${d}" ] ; \
do \
  s="$${d}/samo-lib" ; \
  [ -d "$${s}" ] && echo $${s} && exit 0 ; \
  d=$$(readlink -es "$${d}/..") ; \
done ; \
echo samo_lib_NOT_FOUND ; \
exit 1
endef
SAMO_LIB := $(shell ${FIND_SAMO_LIB})

FATFS_MODE = read-write

include ${SAMO_LIB}/Mk/definitions.mk


INCLUDES += -Isrc
INCLUDES += -I-
INCLUDES += -I${SAMO_LIB_INCLUDE}
INCLUDES += -I${FATFS_INCLUDE} -I${FATFS_CONFIG_INCLUDE}
INCLUDES += -I${DRIVERS_INCLUDE}
INCLUDES += -I${MINI_LIBC_INCLUDE}
INCLUDES += -Ibuild

LIBS += build/libinternal.a
LIBS += ${FATFS_LIB}
LIBS += ${MINI_LIBC_LIB}
LIBS += ${DRIVERS_LIB}
LIBS += $(shell $(CC) -print-libgcc-file-name)

BUILD_PREFIX := build/

MAKE_SYSCALL := scripts/MakeSystemCalls

vpath %.c :src

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

build/libinternal.a: build ${BUILD_OBJECTS}
	${RM} "$@"
	${AR} r "$@" ${BUILD_OBJECTS}

include/grifo.h stubs/stamp-source build/syscall.table: include build stubs src/syscall.list ${MAKE_SYSCALL} src/*.h
	${MAKE_SYSCALL} --prefix=stubs/ \
	  --header=include/grifo.h \
	  --list=src/syscall.list \
	  --table=build/syscall.table \
	  --verbose
	${TOUCH} stubs/stamp-source

stubs/stamp-objects: $(addsuffix .o, $(basename $(wildcard stubs/*.s)))
	${TOUCH} "$@"

stubs/stamp: stubs
	${MAKE} stubs/stamp-objects
	${TOUCH} "$@"

# this should depend on lib, but make always rebuilds in that case; why?
lib/libgrifo.a: lib stubs/stamp stubs/stamp-source
	${RM} "$@"
	${AR} r "$@" stubs/*.o
	${TOUCH} "$@"

grifo.elf: build build/main.o lds/grifo.lds ${LIBS}
	$(LD) -o $@ ${LDFLAGS} build/main.o ${LIBS} -T lds/${@:.elf=.lds} -Map ${@:.elf=.map}
	${STRIP} --strip-unneeded --strip-debug \
		--remove-section=.internal --remove-section=.lcd \
		--remove-section=.user_area --remove-section=.descriptor "$@"
	${OBJDUMP} --section-headers "$@"
	${OBJDUMP} -D "$@" > "${@:.elf=.dump}"

build:
	${MKDIR} "$@"

lib:
	${MKDIR} "$@"

stubs:
	${MKDIR} "$@"

include:
	${MKDIR} "$@"


.PHONY: install
install: all
	@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	${COPY} grifo.elf "${DESTDIR}"/kernel.elf
	${MAKE} -C examples install

.PHONY: clean
clean:
	${RM} -r ${TARGETS} build include lib stubs
	${RM} -r *.o *.elf *.d *.map *.asm33 *.dump
	${MAKE} -C examples clean


include ${SAMO_LIB}/Mk/rules.mk
