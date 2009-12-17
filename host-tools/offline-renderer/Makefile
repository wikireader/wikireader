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


define FIND_MK_DIR
while : ; do \
  d=$$(pwd) ; s="$${d}/samo-lib/Mk" ; \
  [ -d "$${s}" ] && echo $${s} && exit 0 ; \
  [ X"/" = X"$${d}" ] && echo Mk_DIRECTORY_NOT_FOUND && exit 1 ; \
  cd .. ; \
done
endef
MK_DIR := $(shell ${FIND_MK_DIR})
include ${MK_DIR}/definitions.mk

# whilch block to render
RENDER_BLOCK ?= 0
START ?= 1
COUNT ?= all
DESTDIR ?= .
FONT_PATH ?= $(shell ${REALPATH} ${HOST_TOOLS}/fonts)
WORKDIR ?= /tmp

PREFIX := $(shell ${RESOLVEPATH} ${DESTDIR}/pedia)
ARTICLES := $(shell ${RESOLVEPATH} ${WORKDIR}/articles.db)
COUNTS_FILE := $(shell ${RESOLVEPATH} ${WORKDIR}/counts.text)
TEMPLATE_FILE := $(shell ${RESOLVEPATH} ${WORKDIR}/templates.db)
OFFSETS := $(shell ${RESOLVEPATH} ${WORKDIR}/offsets.db)
HTML_ARTICLES := $(shell ${RESOLVEPATH} ${WORKDIR}/articles-${RENDER_BLOCK}.html)

PYLZMA_DIR := pylzma-0.3.0
MATH_DIR := mediawiki-offline/math

$(call REQUIRED_BINARY, sqlite3, sqlite3)
$(call REQUIRED_BINARY, ocamlopt, ocaml)
$(call REQUIRED_BINARY, tex, tetex-extra cjk-latex)
$(call REQUIRED_BINARY, php, php5-cli)
$(call REQUIRED_BINARY, dvipng, dvipng)

$(call REQUIRED_PHP, pdo_sqlite, php5-sqlite)
$(call REQUIRED_PHP, session, php5-session)
$(call REQUIRED_PHP, ctype, php5-ctype)


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
	./ArticleIndex.py ${VERBOSE} \
		--article-index="${ARTICLES}" \
		--article-offsets="${OFFSETS}" \
		--article-counts="${COUNTS_FILE}" \
		--templates="${TEMPLATE_FILE}" \
		--prefix="${PREFIX}" ${XML_FILES}

.PHONY: parse
parse: check-dirs check-html math
	./ArticleParser.py ${VERBOSE} --xhtml="${HTML_ARTICLES}" \
		--start="${START}" --count="${COUNT}" \
		--templates="${TEMPLATE_FILE}" \
		--parser-workdir="${WORKDIR}" \
		--article-offsets="${OFFSETS}"

.PHONY: render
render: pylzma check-dirs check-fonts check-html
	./ArticleRenderer.py ${VERBOSE} --number="${RENDER_BLOCK}" \
		--font-path="${FONT_PATH}" \
		--article-index="${ARTICLES}" \
		--prefix="${PREFIX}" "${HTML_ARTICLES}"

.PHONY: combine
combine: check-dirs
	./combine_idx.py ${VERBOSE} --prefix="${PREFIX}"

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



# sudo apt-get install tetex-extra dvipng cjk-latex ocaml
.PHONY: math
math: stamp-math

# use a lock to avoid multiple parallel builds of this program
stamp-math:
	${RM} "$@"
	${LOCKF} "${@}" ${MAKE} -C "${MATH_DIR}"
	${TOUCH} "$@"

.PHONY: install
install: all

build:
	${MKDIR} "$@"

.PHONY: clean
clean: pylzma-clean
	${RM} -r build ${TARGETS} *.pyc *.pyo *.dat *.idx *.idx-tmp *.pfx *.fnd
	${RM} stamp-*
	${MAKE} -C "${MATH_DIR}" clean

.PHONY: pylzma-clean
pylzma-clean:
	cd ${PYLZMA_DIR} && \
	python setup.py clean && \
	${RM} -r build


include ${MK_DIR}/rules.mk
