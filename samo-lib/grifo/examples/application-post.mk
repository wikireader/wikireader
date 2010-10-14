# Makefile
#
# Copyright (c) 2010 Openmoko Inc.
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

# nomally want simulation
SIMULATE ?= YES

ifeq (,$(strip ${PROGRAM}))
# ensure "PROGRAM = prog-name" is set
$(error PROGRAM variable not set)
endif

TARGETS += ${PROGRAM}.app

ifeq (YES,$(strip ${ICON}))
TARGETS += ${PROGRAM}.ico
endif

# list of sources
ifeq (,$(strip ${SOURCES}))
SOURCES += ${PROGRAM}.c
HEADERS +=
endif

# list of object modules
OBJECTS = ${SOURCES:.c=.o}
BUILD_OBJECTS = $(addprefix ${BUILD_PREFIX},${OBJECTS})

# build application library
lib/libapplication.a: lib ${BUILD_OBJECTS}
	${RM} "$@"
	${AR} r "$@" ${BUILD_OBJECTS}


# build application binary
${PROGRAM}.app: build build/${PROGRAM}.o ${GRIFO_APPLICATION_LDS} ${LIBS}
	$(LD) -o $@ ${LDFLAGS} build/${PROGRAM}.o ${LIBS} -T ${GRIFO_APPLICATION_LDS} -Map ${@:.app=.map}
	${OBJDUMP} -D "$@" > "${@:.app=.dump}"


CLEAN_TARGETS += build
build:
	${MKDIR} "$@"

CLEAN_TARGETS += lib
lib:
	${MKDIR} "$@"


# simulation on host OS

QMAKE_PROJECT := simulate/${PROGRAM}.pro

ifeq (YES,$(strip ${SIMULATE}))
SIMULATE_FILES += $(addprefix ../,${SOURCES})
SIMULATE_FILES += $(addprefix ../,${HEADERS})

SIMULATE_DIR = simulate

TARGETS += simulate-make
CLEAN_TARGETS += ${SIMULATE_DIR}

simulate:
	${MKDIR} "$@"

# prepare a default qmake project file
.PHONY: simulate-files
simulate-files: simulate
	ln -fs "${GRIFO_SIMULATOR}"/* "${GRIFO_COMMON}"/* "${GRIFO_INCLUDE}"/* ${SIMULATE_FILES} "${SIMULATE_DIR}"
	cd "${SIMULATE_DIR}" && \
	qmake -project -o "$(notdir ${QMAKE_PROJECT})"

# this can be overridden by the application makefile
# to modify or append to the ${QMAKE_PROJECT} file
.PHONY: qmake-project
simulate-makefile: simulate-files

# take the project file and convert to a makefile
.PHONY: simulate-makefile
simulate-makefile: simulate simulate-files qmake-project
	cd "${SIMULATE_DIR}" && \
	qmake CONFIG+="qt warn_on thread debug" \
	  QMAKE_CXXFLAGS_WARN_ON+='-Werror' QMAKE_CFLAGS_WARN_ON+='-Werror' \
	  QMAKE_CXXFLAGS+='-DGRIFO_SIMULATOR=1' QMAKE_CFLAGS+='-DGRIFO_SIMULATOR=1' \


# run make on the generated Makefile
.PHONY: simulate-make
simulate-make: simulate simulate-makefile
	${MAKE} -C "simulate"

endif


# no more assignments to TARGETS or CLEAN_TARGETS  after this point
.PHONY: build-targets
build-targets: ${TARGETS}

.PHONY: install
install: all
	@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	${COPY} ${TARGETS} "${DESTDIR}"/

.PHONY: clean
clean:
	${RM} -r ${TARGETS} ${CLEAN_TARGETS}
	${RM} -r *.o *.app *.d *.map *.asm33 *.dump *.ico


include ${MK_DIR}/rules.mk
