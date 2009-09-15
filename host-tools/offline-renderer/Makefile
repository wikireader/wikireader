SAMO_LIB := $(shell readlink -es ../samo-lib || readlink -es ../../samo-lib || readlink -es ../../../samo-lib)

include ${SAMO_LIB}/Mk/definitions.mk

# whilch block to render
RENDER_BLOCK ?= 0
START ?= 1
COUNT ?= all
MODULO ?= 10
DESTDIR ?= .
FONT_PATH ?= ${HOST_TOOLS}/fonts
WORKDIR ?= /tmp

PREFIX := ${DESTDIR}/pedia
ARTICLES := ${WORKDIR}/artcles-${RENDER_BLOCK}.html

PYLZMA_DIR := pylzma-0.3.0

TARGETS =

.PHONY: all
all: index

.PHONY: index
index: pylzma
	@if [ -z "${XML_FILES}" ] ; then echo XML_FILES is not set ; exit 1; fi
	./make_article_idx.py --verbose --modulo="${MODULO}" --prefix="${PREFIX}" ${XML_FILES}

.PHONY: article_idx.py
article_idx.py:
	@echo Run '"'make index'"' first

.PHONY: parse
parse:
	./parse_articles.py --verbose --out="${ARTICLES}" --start="${START}" --count="${COUNT}" ${XML_FILES}
	./make_wrformat.py --verbose --font-path="${FONT_PATH}" --number="${RENDER_BLOCK}" \
		--prefix="${PREFIX}" "${ARTICLES}"

.PHONY: combine
combine:
	./combine_idx.py --verbose --prefix="${PREFIX}"

# sudo apt-get install python-dev
.PHONY: pylzma
pylzma:
	for f in /usr/lib/python*/site-packages/pylzma.so ; \
	do \
	   [ -e "$${f}" ] && exit 0 ; \
	done ; \
	cd ${PYLZMA_DIR} && \
	python setup.py build && \
	sudo python setup.py install

.PHONY: install
install: all
	#@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	#${COPY} *.bmf "${DESTDIR}"/

build:
	${MKDIR} "$@"

.PHONY: clean
clean: pylzma-clean
	${RM} -r build ${TARGETS} article_idx.py *.pyc *.pyo all_articles.html *.dat *.idx *.idx-tmp *.pfx *.fnd

.PHONY: pylzma-clean
pylzma-clean:
	cd ${PYLZMA_DIR} && \
	python setup.py clean && \
	${RM} -r build


include ${SAMO_LIB}/Mk/rules.mk
