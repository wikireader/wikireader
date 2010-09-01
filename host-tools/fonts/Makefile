# Copyright (c) 2009 Openmoko Inc.
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

$(call REQUIRED_BINARY, bdftopcf, xfonts-utils)
$(call REQUIRED_BINARY, xz, xz-utils)

BDFTOPCF = $(shell which bdftopcf)
PCFTOBMF = ${HOST_TOOLS}/pcf2bmf/pcf2bmf

DECOMPRESS = xz --decompress --stdout
COMPRESS = xz --compress --stdout

ENABLE_EXTRA_FONTS ?= YES

STANDARD_FONT = $(eval SOURCE_$(strip ${1}) += $(strip ${2}):$(subst ${SPACE},${COMMA},$(strip ${3}$(if ${4}, -m "$$@"))))


ifneq (,$(findstring y,${ENABLE_EXTRA_FONTS})$(findstring Y,${ENABLE_EXTRA_FONTS}))
EXTRA_FONT = ${STANDARD_FONT}
else
EXTRA_FONT =
endif

TARGETS =
TARGETS += title.bmf
TARGETS += titleall.bmf

TARGETS += subtitle.bmf
TARGETS += subtlall.bmf

TARGETS += text.bmf
TARGETS += textall.bmf
TARGETS += texti.bmf

#TARGETS += textbi.bmf
#TARGETS += textb.bmf
#TARGETS += small.bmf
#TARGETS += textallb.bmf


# define the source files for creating the fonts
# parameters: file.bmf, source.pcf, [<options: -g -c N>], [MERGE]

$(call STANDARD_FONT, text.bmf, textipa.pcf, -c 1152)
$(call STANDARD_FONT, text.bmf, text.pcf, -c 1152, MERGE)

$(call STANDARD_FONT, texti.bmf, texti.pcf, -c 1152)

$(call STANDARD_FONT, textall.bmf, textipa.pcf, -c 65279)
$(call STANDARD_FONT, textall.bmf, textall.pcf, , MERGE)
$(call EXTRA_FONT,    textall.bmf, textall_jp.pcf, -g, MERGE)
$(call EXTRA_FONT,    textall.bmf, textall_kr.pcf, -g, MERGE)

$(call STANDARD_FONT, subtitle.bmf, subtitleipa.pcf, -c 1152)
$(call STANDARD_FONT, subtitle.bmf, subtitle.pcf, -c 1152, MERGE)

$(call STANDARD_FONT, subtlall.bmf, subtitleipa.pcf, -c 65279)
$(call STANDARD_FONT, subtlall.bmf, subtlall.pcf, , MERGE)
$(call EXTRA_FONT,    subtlall.bmf, subtlall_jp.pcf, -g, MERGE)
$(call EXTRA_FONT,    subtlall.bmf, subtlall_kr.pcf, -g, MERGE)

$(call STANDARD_FONT, title.bmf, titleipa.pcf, -c 1152)
$(call STANDARD_FONT, title.bmf, title.pcf, -c 1152, MERGE)

$(call STANDARD_FONT, titleall.bmf, titleipa.pcf, -c 65279)
$(call STANDARD_FONT, titleall.bmf, titleall.pcf, , MERGE)
$(call EXTRA_FONT,    titleall.bmf, titleall_jp.pcf, -g, MERGE)
$(call EXTRA_FONT,    titleall.bmf, titleall_kr.pcf, -g, MERGE)

COMPRESSED_FONTS = textall.pcf.xz
COMPRESSED_FONTS += titleall.pcf.xz
COMPRESSED_FONTS += subtlall.pcf.xz
COMPRESSED_FONTS += subtlall_jp.pcf.xz
COMPRESSED_FONTS += textall_jp.pcf.xz
COMPRESSED_FONTS += titleall_jp.pcf.xz
COMPRESSED_FONTS += subtlall_kr.pcf.xz
COMPRESSED_FONTS += textall_kr.pcf.xz
COMPRESSED_FONTS += titleall_kr.pcf.xz


.PHONY: all
all: stamp-fonts


# use a lock to avoid multiple parallel builds of this program
stamp-fonts:
	${LOCKF} "$@-lock" ${MAKE} ${TARGETS}
	${TOUCH} "$@"


.PHONY: install
install: all
	@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	${COPY} *.bmf "${DESTDIR}"/


# macros to decode SOURCE_xxx
FileNamePart = $(word 1,$(subst :, ,${1}))
OptionsPart = $(subst ${COMMA}, ,$(word 2,$(subst :, ,${1})))

AllFileNames = $(foreach f,${1},$(call FileNamePart,${f}))


# macros to create a font rule
MAKEFONT = $(eval $(call MAKEFONT_1,$(strip ${1})))
define MAKEFONT_1
${1}: $$(call AllFileNames,$${SOURCE_${1}})
	$${RM} "$$@"
	$$(foreach f,$${SOURCE_$(strip $1)},$${PCFTOBMF} -f $$(call FileNamePart,$${f}) $$(call OptionsPart,$${f}) -o "$$@" &&) true
endef

# list of fonts to be created
$(foreach font,${TARGETS},$(call MAKEFONT,${font}))


build:
	${MKDIR} "$@"

.PHONY: compress
compress: ${COMPRESSED_FONTS}

# rebuild the all fonts from source and create compressed fonts
.PHONY: rebuild
rebuild:
	${RM} *.xz
	${MAKE} clean compress all

.PHONY: clean
clean:
	${RM} -r build ${TARGETS} *.bmf *.pcf stamp-*


# fonts
%.pcf: %.bdf
	${BDFTOPCF} "$<" -o "$@"

%.pcf: %.pcf.xz
	${DECOMPRESS} < "$<" > "$@"

%.pcf: %.pcf.src
	${COPY} "$<" "$@"

%.pcf.xz : %.bdf
	${RM} "$@.temp"
	${BDFTOPCF} "$<" -o "$@.temp"
	${COMPRESS} < "$@.temp" > "$@"
	${RM} "$@.temp"

include ${MK_DIR}/rules.mk
