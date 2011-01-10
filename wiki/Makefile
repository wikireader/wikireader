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


include ${GRIFO_APPLICATION_PRE}

PROGRAM = wiki
ICON = YES

# some configuration items
# enable the progress bar by adding:
# PROGRESS_BAR=yes to make command line
ifeq (YES,$(strip ${PROGRESS_BAR}))
ENABLE_PROGRESS := 1
endif
ifeq (yes,$(strip ${PROGRESS_BAR}))
ENABLE_PROGRESS := 1
endif

# default values are disabled
ENABLE_PROGRESS ?= 0

# optional items for compiler
CFLAGS += -DENABLE_PROGRESS="${ENABLE_PROGRESS}"

# enable the temperature display by adding:
# TEMPERATURE_DISPLAY=yes to make command line
ifeq (YES,$(strip ${TEMPERATURE_DISPLAY}))
ENABLE_TEMPERATURE := 1
endif
ifeq (yes,$(strip ${TEMPERATURE_DISPLAY}))
ENABLE_TEMPERATURE := 1
endif

# default values are disabled
ENABLE_TEMPERATURE ?= 0

# optional items for compiler
CFLAGS += -DENABLE_TEMPERATURE="${ENABLE_TEMPERATURE}"

# list of sources and headers
SOURCES += ${PROGRAM}.c
SOURCES += Alloc.c
SOURCES += bigram.c
SOURCES += bmf.c
SOURCES += Bra.c
SOURCES += btree.c
SOURCES += glyph.c
SOURCES += guilib.c
SOURCES += history.c
SOURCES += highlight.c
SOURCES += keyboard.c
SOURCES += languages.c
SOURCES += lcd_buf_draw.c
SOURCES += LzFind.c
SOURCES += LzmaDec.c
SOURCES += restricted.c
SOURCES += search.c
SOURCES += search_fnd.c
SOURCES += sha1.c
SOURCES += utf8.c
SOURCES += wiki_info.c
SOURCES += wikilib.c

HEADERS += Alloc.h
HEADERS += bigram.h
HEADERS += bmf.h
HEADERS += Bra.h
HEADERS += btree.h
HEADERS += general_header.h
HEADERS += glyph.h
HEADERS += guilib.h
HEADERS += highlight.c
HEADERS += history.h
HEADERS += keyboard.h
HEADERS += languages.h
HEADERS += lcd_buf_draw.h
HEADERS += LzFind.h
HEADERS += LzHash.h
HEADERS += LzmaDec.h
HEADERS += mapping_tables.h
HEADERS += restricted.h
HEADERS += search_fnd.h
HEADERS += search.h
HEADERS += sha1.h
HEADERS += Types.h
HEADERS += utf8.h
HEADERS += wiki_info.h
HEADERS += wikilib.h

IMAGES += keyboard_123.png
IMAGES += keyboard_abc2_jp.png
IMAGES += keyboard_abc2_ko.png
IMAGES += keyboard_abc_da.png
IMAGES += keyboard_abc_ko.png
IMAGES += keyboard_abc.png
IMAGES += keyboard_phone_jp_123.png
IMAGES += keyboard_phone_jp_abc.png
IMAGES += keyboard_phone_jp.png
IMAGES += keyboard_phone_tw_123.png
IMAGES += keyboard_phone_tw_abc.png
IMAGES += keyboard_phone_tw.png
IMAGES += nls.png

ALL_IMAGES = $(addprefix ${BUILD_PREFIX},all_images.h)
PREBUILD_TARGETS += ${ALL_IMAGES}

IMAGE_HEADERS = $(addsuffix .h,$(addprefix ${BUILD_PREFIX},$(basename ${IMAGES})))
CLEAN_TARGETS += ${IMAGE_HEADERS}

${ALL_IMAGES}: build ${IMAGE_HEADERS}
	@${RM} "$@"
	@for f in ${IMAGE_HEADERS}; \
	 do \
	   echo Keyboard: $$(basename "$${f}") ; \
	   echo '#include "'$$(basename "$${f}")'"' >> "$@"; \
	 done

SIMULATE_FILES += $(addprefix ../,${IMAGES})

qmake-project:
	echo 'all_images.target = all_images.h' >> ${QMAKE_PROJECT}
	echo 'all_images.commands = '"'"'@$${DEL_FILE} "$$@";'"'" >> ${QMAKE_PROJECT}
	for h in $(notdir ${IMAGE_HEADERS}); \
	do \
	  t="$${h%.h}"; \
	  png="$${t}.png"; \
	  echo "all_images.commands += 'echo Keyboard: $${h};'" >> ${QMAKE_PROJECT}; \
	  echo "all_images.commands += 'echo \'\$$\$${LITERAL_HASH}include \"$${h}\"\' >> \"\$$@\";'" >> ${QMAKE_PROJECT}; \
	  echo "all_images.depends += $${h}" >> ${QMAKE_PROJECT}; \
	  echo "$${t}.target = $${h}" >> ${QMAKE_PROJECT}; \
	  echo "$${t}.depends = $${png}" >> ${QMAKE_PROJECT}; \
	  echo "$${t}.commands = '${IMAGE2HEADER} --inverted --header=$${h} --variable=$${t}_image $${png}';" >> ${QMAKE_PROJECT}; \
	  echo "QMAKE_EXTRA_TARGETS += $${t}" >> ${QMAKE_PROJECT}; \
	done
	echo 'QMAKE_EXTRA_TARGETS += all_images' >> ${QMAKE_PROJECT}
	echo 'QMAKE_LIBS += -lrt' >> ${QMAKE_PROJECT}
	echo 'QMAKE_CLEAN += $$$${all_images.target}' >> ${QMAKE_PROJECT}


# this must be at the end
include ${GRIFO_APPLICATION_POST}
