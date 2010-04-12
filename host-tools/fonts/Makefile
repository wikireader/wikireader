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

TARGETS = title.bmf
TARGETS += subtitle.bmf
TARGETS += texti.bmf
#TARGETS += textbi.bmf
TARGETS += text.bmf
#TARGETS += textb.bmf
#TARGETS += small.bmf
TARGETS += textall.bmf
#TARGETS += textallb.bmf
TARGETS += titleall.bmf
TARGETS += subtlall.bmf

COMPRESSED_FONTS = textall.pcf.xz
COMPRESSED_FONTS += titleall.pcf.xz
COMPRESSED_FONTS += subtlall.pcf.xz


.PHONY: all
all: stamp-fonts

# use a lock to avoid multiple parallel builds of this program
stamp-fonts:
	${LOCKF} "$@" ${MAKE} ${TARGETS}
	${TOUCH} "$@"


.PHONY: install
install: all
	@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	${COPY} *.bmf "${DESTDIR}"/

build:
	${MKDIR} "$@"

.PHONY: compress
compress: ${COMPRESSED_FONTS}

.PHONY: clean
clean:
	${RM} -r build ${TARGETS} *.bmf *.pcf stamp-*


# fonts
%.pcf: %.bdf
	${BDFTOPCF} "$<" -o "$@"

%.bmf: %.pcf
	${PCFTOBMF} -f "$<" -o "$@" -c 1152

%.bmf: %.pcf.xz
	${DECOMPRESS} "$<" | ${PCFTOBMF} -f - -o "$@"

%.pcf.xz : %.bdf
	${RM} "$@.temp"
	${BDFTOPCF} "$<" -o "$@.temp"
	${COMPRESS} < "$@.temp" > "$@"
	${RM} "$@.temp"

include ${MK_DIR}/rules.mk
