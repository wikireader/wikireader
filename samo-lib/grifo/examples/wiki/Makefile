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


INCLUDES += -I${SAMO_LIB_INCLUDE}
INCLUDES += -I${GRIFO_INCLUDE}
INCLUDES += -I${MINI_LIBC_INCLUDE}

LIBS += lib/libexamples.a
LIBS += ${GRIFO_LIB}
LIBS += ${MINI_LIBC_LIB}
LIBS += $(shell $(CC) -print-libgcc-file-name)

LDFLAGS += -static --strip-all -s --no-gc-sections -N

BUILD_PREFIX := build/

INCLUDES += -I${BUILD_PREFIX}

vpath %.c :src
vpath %.png :src

ALL_IMAGES = $(addprefix ${BUILD_PREFIX},all_images.h)

TARGETS += wiki.app wiki.ico

# list of modules
OBJECTS = main.o
OBJECTS += guilib.o
OBJECTS += bmf.o
OBJECTS += lcd_buf_draw.o
OBJECTS += bigram.o
OBJECTS += glyph.o
OBJECTS += restricted.o
OBJECTS += sha1.o
OBJECTS += wiki_info.o
OBJECTS += wikilib.o
OBJECTS += keyboard.o
OBJECTS += utf8.o
OBJECTS += btree.o
OBJECTS += languages.o
OBJECTS += search.o
OBJECTS += search_fnd.o
OBJECTS += history.o
OBJECTS += LzmaDec.o
OBJECTS += Alloc.o
OBJECTS += Bra.o
OBJECTS += LzFind.o

# convert the image files
IMAGES = keyboard_abc keyboard_abc2_jp keyboard_abc2_ko keyboard_abc_ko keyboard_abc_da keyboard_123 nls
IMAGES += keyboard_phone_jp keyboard_phone_jp_abc keyboard_phone_jp_123
IMAGES += keyboard_phone_tw keyboard_phone_tw_abc keyboard_phone_tw_123

BUILD_OBJECTS = $(addprefix ${BUILD_PREFIX},${OBJECTS})

.PHONY: all
all:  ${TARGETS}

.PHONY: pre-build
pre-build: ${ALL_IMAGES}

lib/libexamples.a: lib pre-build ${BUILD_OBJECTS}
	${RM} "$@"
	${AR} r "$@" ${BUILD_OBJECTS}


wiki.app: build build/main.o ${GRIFO_APPLICATION_LDS} ${LIBS}
	$(LD) -o $@ ${LDFLAGS} build/main.o ${LIBS} -T ${GRIFO_APPLICATION_LDS} -Map ${@:.app=.map}
	${OBJDUMP} -D "$@" > "${@:.app=.dump}"

IMAGE_HEADERS = $(addsuffix .h,$(addprefix ${BUILD_PREFIX},$(strip ${IMAGES})))
CLEAN_FILES += ${IMAGE_HEADERS}
${ALL_IMAGES}: build ${IMAGE_HEADERS}
	@${RM} "$@"
	@for f in ${IMAGE_HEADERS}; \
	 do \
	   echo Keyboard: $$(basename "$${f}") ; \
	   echo '#include "'$$(basename "$${f}")'"' >> "$@"; \
	 done

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
	${RM} -r *.o *.app *.d *.map *.asm33 *.dump *.ico


include ${MK_DIR}/rules.mk
