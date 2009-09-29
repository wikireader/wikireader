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
