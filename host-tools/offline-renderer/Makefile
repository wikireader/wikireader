SAMO_LIB := $(shell readlink -es ../samo-lib || readlink -es ../../samo-lib || readlink -es ../../../samo-lib)

include ${SAMO_LIB}/Mk/definitions.mk

# whilch block to render
RENDER_BLOCK ?= 0
START ?= 1
COUNT ?= all
MODULO ?= 10
DESTDIR ?= .
FONT_PATH ?= $(shell readlink -m ${HOST_TOOLS}/fonts)
WORKDIR ?= /tmp

PREFIX := $(shell readlink -m ${DESTDIR}/pedia)
ARTICLES := $(shell readlink -m ${WORKDIR}/articles.pickle)
COUNTS_FILE := $(shell readlink -m ${WORKDIR}/counts.text)
OFFSETS := $(shell readlink -m ${WORKDIR}/offsets.pickle)
HTML_ARTICLES := $(shell readlink -m ${WORKDIR}/articles-${RENDER_BLOCK}.html)

PYLZMA_DIR := pylzma-0.3.0

ifneq ($(strip ${VERBOSE}),)
VERBOSE = --verbose
endif

TARGETS = index parse render combine

.PHONY: all
all: ${TARGETS}

.PHONY: check-xml
check-xml:
	@if [ -z "${XML_FILES}" ] ; then echo XML_FILES is not set ; exit 1; fi

.PHONY: check-dirs
check-dirs:
	@if [ ! -d "${DESTDIR}" ] ; then echo DESTDIR: "'"${DESTDIR}"'" is not a directory ; exit 1; fi
	@if [ ! -d "${WORKDIR}" ] ; then echo WORKDIR: "'"${WORKDIR}"'" is not a directory ; exit 1; fi

.PHONY: check-fonts
check-fonts:
	@if [ ! -d "${FONT_PATH}" ] ; then echo FONT_PATH: "'"${FONT_PATH}"'" is not a directory ; exit 1; fi

.PHONY: check-html
check-html:
	@if [ -z "${HTML_ARTICLES}" ] ; then echo HTML_ARTICLES is not set ; exit 1; fi


.PHONY: index
index: check-dirs check-xml
	time ./make_article_idx.py ${VERBOSE} \
		--article-index="${ARTICLES}" --article-offsets="${OFFSETS}" \
		--article-counts="${COUNTS_FILE}" \
		--modulo="${MODULO}" --prefix="${PREFIX}" ${XML_FILES}

.PHONY: parse
parse: check-dirs check-xml check-html
	time ./parse_articles.py ${VERBOSE} --xhtml="${HTML_ARTICLES}" \
		--start="${START}" --count="${COUNT}" \
		--article-offsets="${OFFSETS}" \
		${XML_FILES}

.PHONY: render
render: pylzma check-dirs check-fonts check-html
	time ./make_wrformat.py ${VERBOSE} --number="${RENDER_BLOCK}" \
		--font-path="${FONT_PATH}" \
		--article-index="${ARTICLES}" \
		--prefix="${PREFIX}" "${HTML_ARTICLES}"

.PHONY: combine
combine: check-dirs
	time ./combine_idx.py ${VERBOSE} --prefix="${PREFIX}"

# sudo apt-get install python-dev
.PHONY: pylzma
pylzma:
	for dir in /usr/lib/python* /usr/local/lib/python* /opt/lib/python* ; \
	do \
	   [ -e "$${dir}/site-packages/pylzma.so" ] && exit 0 ; \
	   [ -e "$${dir}/dist-packages/pylzma.so" ] && exit 0 ; \
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
