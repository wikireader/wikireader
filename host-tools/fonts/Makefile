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


SAMO_LIB := $(shell readlink -es ../samo-lib || readlink -es ../../samo-lib || readlink -es ../../../samo-lib)
include ${SAMO_LIB}/Mk/definitions.mk


BDFTOPCF = ${shell which bdftopcf}
PCFTOBMF = ${HOST_TOOLS}/pcf2bmf/pcf2bmf

ifeq (${strip ${BDFTOPCF}},)
${error missing: bdftopcf; sudo apt-get install xfonts-utils }
endif


TARGETS = title.bmf
TARGETS += subtitle.bmf
TARGETS += texti.bmf
#TARGETS += textbi.bmf
TARGETS += text.bmf
#TARGETS += textb.bmf
#TARGETS += small.bmf
TARGETS += textall.bmf
#TARGETS += textallb.bmf



.PHONY: all
all: ${TARGETS}


.PHONY: install
install: all
	@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	${COPY} *.bmf "${DESTDIR}"/

build:
	${MKDIR} "$@"

.PHONY: clean
clean:
	${RM} -r build ${TARGETS} *.bmf *.pcf

# fonts
%.pcf: %.bdf
	${BDFTOPCF} "$<" -o "$@"

%.bmf: %.pcf
	${PCFTOBMF} -f "$<" -o "$@"

include ${SAMO_LIB}/Mk/rules.mk
