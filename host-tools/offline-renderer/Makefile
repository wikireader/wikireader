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


# which block to render
RENDER_BLOCK ?= 0
START ?= 1
COUNT ?= all
TOTAL_HTML_FILES ?= 64
DESTDIR ?= .
FONT_PATH ?= $(shell ${REALPATH} ${HOST_TOOLS}/fonts)
WORKDIR ?= /tmp
TEMPDIR ?= ${WORKDIR}/tmp
WIKI_FILE_PREFIX ?= wiki
WIKI_LANGUAGE ?= en
WIKI_DIR_SUFFIX ?= pedia
ENABLE_LANGUAGES_LINKS ?= YES
ENABLE_IMAGES ?= YES
ARTICLES_PER_BLOCK ?= 32
ARTICLE_BLOCK_SIZE ?= 262144

WORKDIR_PATH := $(shell ${RESOLVEPATH} ${WORKDIR}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX})
TEMPDIR_PATH := $(shell ${RESOLVEPATH} ${TEMPDIR}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX})

PREFIX := $(shell ${RESOLVEPATH} ${DESTDIR}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX}/${WIKI_FILE_PREFIX})
ARTICLES := $(shell ${RESOLVEPATH} ${WORKDIR_PATH}/articles.db)
COUNTS_FILE := $(shell ${RESOLVEPATH} ${WORKDIR_PATH}/counts.text)
TEMPLATE_FILE := $(shell ${RESOLVEPATH} ${WORKDIR_PATH}/templates.db)
OFFSETS := $(shell ${RESOLVEPATH} ${WORKDIR_PATH}/offsets.db)
HTML_ARTICLES_PREFIX := $(shell ${RESOLVEPATH} ${WORKDIR_PATH}/articles-)
HTML_ARTICLES := $(shell ${RESOLVEPATH} ${HTML_ARTICLES_PREFIX}${RENDER_BLOCK}.html)

PYLZMA_DIR := pylzma-0.3.0
MATH_DIR := mediawiki-offline/math
MEDIAWIKI_MESSAGES_DIR := mediawiki-offline/languages/messages

$(call REQUIRED_BINARY, sqlite3, sqlite3)
$(call REQUIRED_BINARY, ocamlopt, ocaml)
$(call REQUIRED_BINARY, tex, cjk-latex)
$(call REQUIRED_BINARY, php, php5-cli)
$(call REQUIRED_BINARY, dvipng, dvipng)

$(call REQUIRED_PHP, pdo_sqlite, php5-sqlite)
$(call REQUIRED_PHP, session, php5-session)
$(call REQUIRED_PHP, ctype, php5-ctype)


ifeq (yes,$(strip ${VERBOSE}))
VERBOSE_ARG = --verbose
endif
ifeq (YES,$(strip ${VERBOSE}))
VERBOSE_ARG = --verbose
endif

TRUNCATE_ARG = --truncate-title
ifneq (yes,$(strip ${ENABLE_LANGUAGE_LINKS}))
TRUNCATE_ARG =
endif
ifneq (YES,$(strip ${ENABLE_LANGUAGE_LINKS}))
TRUNCATE_ARG =
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
	@if [ ! -d "${WORKDIR_PATH}" ] ; then echo WORKDIR_PATH: "'"${WORKDIR_PATH}"'" is not a directory ; exit 1; fi
	@[ -d "${TEMPDIR}" ] || ${MKDIR} "${TEMPDIR}"
	@if [ ! -d "${TEMPDIR}" ] ; then echo TEMPDIR: "'"${TEMPDIR}"'" is not a directory ; exit 1; fi

.PHONY: check-fonts
check-fonts:
	@if [ ! -d "${FONT_PATH}" ] ; then echo FONT_PATH: "'"${FONT_PATH}"'" is not a directory ; exit 1; fi

.PHONY: check-html
check-html:
	@if [ -z "${HTML_ARTICLES}" ] ; then echo HTML_ARTICLES is not set ; exit 1; fi


.PHONY: index
index: check-dirs check-xml stamp-RedirectedTo.py stamp-PinyinTable.py
	./ArticleIndex.py ${VERBOSE_ARG} \
		--article-index="${ARTICLES}" \
		--article-offsets="${OFFSETS}" \
		--article-counts="${COUNTS_FILE}" \
		--templates="${TEMPLATE_FILE}" \
		--language="${WIKI_LANGUAGE}" \
		--prefix="${PREFIX}" ${TRUNCATE_ARG} ${XML_FILES}

HTML_FILES_COUNT := $(words $(wildcard ${WORKDIR_PATH}/*.html))
.PHONY: merge
merge:
ifeq (0,$(strip $(shell [ "${HTML_FILES_COUNT}" -gt "${TOTAL_HTML_FILES}" ] ; echo $$?)))
	@echo merging: ${HTML_FILES_COUNT} to: ${TOTAL_HTML_FILES}
	@${RM} ${HTML_ARTICLES_PREFIX}*.html-UNMERGED
	@i=0 ; \
	while [ $${i} -lt ${HTML_FILES_COUNT} ] ; \
	do \
	  f=${HTML_ARTICLES_PREFIX}$${i}.html ; \
	  mv "$${f}" "$${f}-UNMERGED" ; \
	  i=$$(($${i} + 1)) ; \
	done
	@i=0 ; in=0 ; out=0 ; \
	total=${TOTAL_HTML_FILES} ; count=${HTML_FILES_COUNT} ; \
	while [ $${out} -lt ${TOTAL_HTML_FILES} ] ; \
	do \
	  step=$$(expr '(' 2 '*' $${count} / $${total} + 1 ')' / 2) ; \
	  count=$$(($${count} - $${step})) ; \
	  f_out=${HTML_ARTICLES_PREFIX}$${out}.html ; \
	  ${RM} $${f_out} ; \
	  echo -n generating: $${f_out} ; \
	  while [ $${step} -gt 0 ] ; \
	  do \
	    f=${HTML_ARTICLES_PREFIX}$${in}.html-UNMERGED ; \
	    echo -n " $${in}" ; \
	    cat "$${f}" >> "$${f_out}" ; \
	    in=$$(($${in} + 1)) ; \
	    step=$$(($${step} - 1)) ; \
	  done ; \
	  echo ; \
	  out=$$(($${out} + 1)) ; \
	  total=$$(($${total} - 1)) ; \
	done
else
	@echo Merge Ignored: insufficient files: ${HTML_FILES_COUNT} '<=' ${TOTAL_HTML_FILES}
endif

.PHONY: parse
parse: check-dirs check-html stamp-math stamp-RedirectedTo.py
	./ArticleParser.py ${VERBOSE_ARG} --xhtml="${HTML_ARTICLES}" \
		--language="${WIKI_LANGUAGE}" \
		--start="${START}" --count="${COUNT}" \
		--templates="${TEMPLATE_FILE}" \
		--parser-workdir="${WORKDIR_PATH}" \
		--parser-tempdir="${TEMPDIR_PATH}" \
		--article-offsets="${OFFSETS}"

.PHONY: render
render: check-dirs check-fonts check-html stamp-PinyinTable.py
	./ArticleRenderer.py ${VERBOSE_ARG} --number="${RENDER_BLOCK}" \
		--font-path="${FONT_PATH}" \
		--article-index="${ARTICLES}" \
		--prefix="${PREFIX}" \
		--language-links="${ENABLE_LANGUAGES_LINKS}" \
		--images="${ENABLE_IMAGES}" \
		--articles="${ARTICLES_PER_BLOCK}" \
		--block-size="${ARTICLE_BLOCK_SIZE}" \
		"${HTML_ARTICLES}"

.PHONY: combine
combine: check-dirs
	./combine_idx.py ${VERBOSE_ARG} --prefix="${PREFIX}"

# sudo apt-get install python-dev
.PHONY: pylzma
pylzma:
	cd ${PYLZMA_DIR} && \
	python setup.py build && \
	sudo python setup.py install


# create the #redirect regular expression
CLEAN_TARGETS += RedirectedTo.py
RedirectedTo.py: FindAllRedirects.php ${MEDIAWIKI_MESSAGES_DIR}/*.php
	php5 FindAllRedirects.php ${MEDIAWIKI_MESSAGES_DIR}/*.php > "$@"

# create Pinyin table for index program
PINYIN_TABLE = pinyin-data/pinyin_table.txt
CLEAN_TARGETS += PinyinTable.py
PinyinTable.py: CreatePinyinTable.py
	./CreatePinyinTable.py --input="${PINYIN_TABLE}" --output="$@"


# sudo apt-get install tetex-extra dvipng cjk-latex ocaml
.PHONY: math
math:
	${MAKE} -C "${MATH_DIR}"

# use a lock to avoid multiple parallel builds of specific programs
# i.e depend on "stamp-program" instead of "program"
# so that the actual build will be serialised in a recursive call to make
stamp-%:
	${LOCKF} "$@" ${MAKE} "${@:stamp-%=%}"
	${TOUCH} "$@"


.PHONY: install
install: all

build:
	${MKDIR} "$@"

.PHONY: clean
clean: pylzma-clean
	${RM} -r build ${TARGETS} ${CLEAN_TARGETS} *.pyc *.pyo *.dat *.idx *.idx-tmp *.pfx *.fnd
	${RM} stamp-*
	${MAKE} -C "${MATH_DIR}" clean

.PHONY: pylzma-clean
pylzma-clean:
	cd ${PYLZMA_DIR} && \
	python setup.py clean && \
	${RM} -r build


include ${MK_DIR}/rules.mk
