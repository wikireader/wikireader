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


# macros for font building

STANDARD_FONT = $(eval $(call STANDARD_FONT1,$(strip ${1}),$(strip ${2}),$(strip ${3}),$(strip ${4})))

define STANDARD_FONT1
SOURCE_$(addsuffix .bmf,${1}) += $(addsuffix .pcf,${2}):$(subst ${SPACE},${COMMA},$(strip ${3} $(if ${4}, -m "$$@")))
FONT_TARGETS += $(addsuffix .bmf,${1})
COMPRESSED_FONTS += $(addsuffix .pcf.xz,${2})
endef

TARGETS =$(sort ${FONT_TARGETS})

ifneq (,$(findstring y,${ENABLE_EXTRA_FONTS})$(findstring Y,${ENABLE_EXTRA_FONTS}))
EXTRA_FONT = ${STANDARD_FONT}
else
EXTRA_FONT =
endif


# define the source files for creating the fonts
# parameters: target, source, [<options: -g -c N>], [MERGE]
#   target .bmf suffix is assumed
#   source .pcf suffix is assumed

$(call STANDARD_FONT, text, textipa, -c 1152)
$(call STANDARD_FONT, text, text, -c 1152, MERGE)

$(call STANDARD_FONT, texti, texti, -c 1152)

$(call STANDARD_FONT, textall, textipa)
$(call EXTRA_FONT,    textall, textall_kr, , MERGE)
$(call STANDARD_FONT, textall, textall, , MERGE)
$(call EXTRA_FONT,    textall, textall_jp, -g, MERGE)

$(call STANDARD_FONT, subtitle, subtitleipa, -c 1152)
$(call STANDARD_FONT, subtitle, subtitle, -c 1152, MERGE)

$(call STANDARD_FONT, subtlall, subtitleipa)
$(call EXTRA_FONT,    subtlall, subtlall_kr, , MERGE)
$(call STANDARD_FONT, subtlall, subtlall, , MERGE)
$(call EXTRA_FONT,    subtlall, subtlall_jp, -g, MERGE)

$(call STANDARD_FONT, title, titleipa, -c 1152)
$(call STANDARD_FONT, title, title, -c 1152, MERGE)

$(call STANDARD_FONT, titleall, titleipa)
$(call EXTRA_FONT,    titleall, titleall_kr_unicode, , MERGE)
$(call EXTRA_FONT,    titleall, titleall_kr, , MERGE)
$(call STANDARD_FONT, titleall, titleall, , MERGE)
$(call EXTRA_FONT,    titleall, titleall_jp, -g, MERGE)


# main target
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
	$$(foreach f,$${SOURCE_$(strip $1)},$${PCFTOBMF} -f $$(call FileNamePart,$${f}) $$(call OptionsPart,$${f}) -o "$$@" &&) true || $${RM} "$$@"
	@[ -e "$$@" ]
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
