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

include ${SAMO_LIB}/Mk/definitions.mk


INCLUDES += -I${SAMO_LIB_INCLUDE}
INCLUDES += -I${GRIFO_INCLUDE}
INCLUDES += -I${MINI_LIBC_INCLUDE}

LIBS += lib/libexamples.a
LIBS += ${GRIFO_LIB}
LIBS += ${MINI_LIBC_LIB}
LIBS += $(shell $(CC) -print-libgcc-file-name)

BUILD_PREFIX := build/

vpath %.c :src

TARGETS = sample.app

# list of modules
OBJECTS += sample.o

BUILD_OBJECTS = $(addprefix ${BUILD_PREFIX},${OBJECTS})

.PHONY: all
all:  ${TARGETS}

lib/libexamples.a: lib ${BUILD_OBJECTS}
	${RM} "$@"
	${AR} r "$@" ${BUILD_OBJECTS}


sample.app: build build/sample.o ${GRIFO_APPLICATION_LDS} ${LIBS}
	$(LD) -o $@ ${LDFLAGS} build/sample.o ${LIBS} -T ${GRIFO_APPLICATION_LDS} -Map ${@:.app=.map}
	${OBJDUMP} -D "$@" > "${@:.app=.dump}"


build:
	${MKDIR} "$@"

lib:
	${MKDIR} "$@"

.PHONY: install
install: all
	@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	${COPY} ${TARGETS} "${DESTDIR}"/

.PHONY: clean
clean:
	${RM} -r ${TARGETS} build lib
	${RM} -r *.o *.app *.d *.map *.asm33 *.dump


include ${SAMO_LIB}/Mk/rules.mk
