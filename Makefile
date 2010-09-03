# Makefile - a Makefile for setting up Wiki Reader
#
# (C) Copyright 2008, 2009 Openmoko, Inc.
# Authors: xiangfu liu <xiangfu@openmoko.org>
#          Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# version 3 as published by the Free Software Foundation.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA

# Control variables
# =================

# These  can be overridden on the command line:
# (defaults indicated in [])
#
#   WIKI_LANGUAGE              Language code [en]
#
#   WIKI_FILE_PREFIX           Prefix for all files in a data directory [wiki]
#
#   WIKI_VERSION               Version code for the current rendering operation [${todays-date}]
#
#   WIKI_DIR_SUFFIX            Combined with language code to make a unique
#                              directory name [pedia]
#
#   DESTDIR                    Directory where rendered data will be stored
#
#   WORKDIR                    Directory where all the working databases are stored
#
#   TEMPDIR                    Optional directory to store LaTeX work files, point
#                              to a RAM disk [derived from ${WORKDIR}]
#
#   VERSION_TAG                Optional version string for the root programs/fonts [${todays-date}]
#
#   PROGRESS_BAR               Enable progress bar when compiling mahatma.elf [NO]
#
#   EXTRACT_VERSION_FROM       a link to XML file name that has an embedded date
#                              which will be used to set WIKI_VERSION
#
#   MACHINE_COUNT              Number of machines in the build cluster [9]
#
#   PARALLEL_BUILD             Number of threads/cores usable on each machine [3]
#
# Items controlling the compression - for reducing data size to fit smaller SD Card
#
#   ENABLE_LANGUAGES_LINKS     Enable links to other languages [YES]
#
#   ENABLE_IMAGES              Enable embedded images (Formulae) [YES]
#
#   ARTICLES_PER_BLOCK         Maximum number of articles to group in one block [1] (max 255)
#                              Larger number will give greater compression, but much slower load times
#
#   ARTICLE_BLOCK_SIZE         Size limit of compression block [262144] (max 524288)
#                              Only applies when ARTICLES_PER_BLOCK > 1
#
#   MAX_ARTICLE_LENGTH         Shorten articles over this length (length of rendered byte code) [UNLIMITED]


# Include standard definitions
# ============================

CROSS_COMPILER := NO

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


# Toolchain configuration data
# ============================

GCC_VERSION = 3.3.2
GCC_PACKAGE = gcc-${GCC_VERSION}.tar.gz
GCC_URL = ftp://ftp.gnu.org/gnu/gcc/${GCC_PACKAGE}

BINUTILS_VERSION = 2.10.1
BINUTILS_PACKAGE = binutils-${BINUTILS_VERSION}.tar.gz
BINUTILS_URL= ftp://ftp.gnu.org/gnu/binutils/${BINUTILS_PACKAGE}

DOWNLOAD_DIR = ${HOST_TOOLS}/toolchain-download
SUM_DIR = ${HOST_TOOLS}/toolchain-sums

CONFIG_FILE := "${SAMO_LIB}/include/config.h"
CONFIG_FILE_DEFAULT := "${SAMO_LIB}/include/config.h-default"
CONFIG_FILE_EXISTS := $(shell [ -f "${CONFIG_FILE}" ] && echo 1)

ifeq (${CONFIG_FILE_EXISTS},)
$(shell cp ${CONFIG_FILE_DEFAULT} ${CONFIG_FILE})
$(error edit ${CONFIG_FILE} file and re-run make)
endif


# macro to simplify primary targets
# =================================

STD_BASE = $(eval $(call STD_BASE1,$(strip ${1}),$(strip ${2}),$(strip ${3}),$(strip ${4})))
STD_RULE = ${STD_BASE}$(eval $(call STD_RULE1,$(strip ${1}),$(strip ${2}),$(strip ${3}),$(strip ${4}),$(strip ${5})))

define STD_RULE1

.PHONY: ${1}
${1}: ${3}
	$${MAKE} -C "${2}" all ${5}

ALL_TARGETS += ${1}

ifeq (INSTALL,${4})
.PHONY: ${1}-install
${1}-install: ${1} validate-destdir
	${MAKE} -C "${2}" DESTDIR="$${DESTDIR_PATH}" install

INSTALL_TARGETS += ${1}-install
endif

endef

define STD_BASE1
.PHONY: ${1}-clean
${1}-clean:
	$${MAKE} -C "${2}" clean

CLEAN_TARGETS += ${1}-clean

.PHONY: ${1}-requires
${1}-requires:
	$${MAKE} -C "${2}" requires

REQUIRES_TARGETS += ${1}-requires

endef


# Makefile entry point
# ====================

ALL_TARGETS =

.PHONY: all
all:    all-targets


# wiki naming
# ===========

EXTRACT_VERSION_PATH := $(realpath ${EXTRACT_VERSION_FROM})
ifneq (,$(strip ${EXTRACT_VERSION_PATH}))
WIKI_VERSION := $(subst .,,$(suffix $(subst -,.,$(patsubst %-pages-articles,%,$(notdir $(basename ${EXTRACT_VERSION_PATH}))))))
endif

WIKI_LANGUAGE ?= en
WIKI_FILE_PREFIX ?= wiki
WIKI_DIR_SUFFIX ?= pedia
WIKI_VERSION ?= $(shell date '+%Y%m%d')


# Installation
# ============

DESTDIR_PATH := $(abspath ${DESTDIR})
WORKDIR_PATH := $(abspath ${WORKDIR})
TEMPDIR_PATH := $(abspath ${TEMPDIR})
ifeq (,$(strip ${TEMPDIR_PATH}))
TEMPDIR_PATH := ${WORKDIR_PATH}/tmp
endif

VERSION_TAG ?= $(shell date '+%Y%m%d')
VERSION_FILE := ${DESTDIR_PATH}/version.txt
SHA_LEVEL := 256
CHECKSUM_FILE := sha${SHA_LEVEL}.txt

.PHONY: jig-install
jig-install: validate-destdir forth-install flash-install mbr-install

.PHONY: install
install: validate-destdir mahatma-install forth-install fonts-install nls-install misc-files-install version

.PHONY: version
version: validate-destdir
	@if [ -z "${VERSION_TAG}" ] ; then echo VERSION_TAG: "'"${VERSION_TAG}"'" is not valid ; exit 1; fi
	${RM} "${VERSION_FILE}" "${DESTDIR_PATH}"/*.idx-tmp "${DESTDIR_PATH}"/*~
	${RM} "${DESTDIR_PATH}"/*/*.idx-tmp
	echo VERSION: ${VERSION_TAG} >> "${VERSION_FILE}"
	${RM} "${DESTDIR_PATH}"/[wW][iI][kK][iI].[pP][aA][sS]
	${RM} "${DESTDIR_PATH}"/[wW][iI][kK][iI].[hH][sS][tT]
	${RM} "${DESTDIR_PATH}"/[wW][iI][kK][iI].[iI][nN][iI]
	find "${DESTDIR_PATH}" -type d -print -exec \
	  sh -c "cd '{}' && ${RM} '${CHECKSUM_FILE}' && sha${SHA_LEVEL}sum * > '${CHECKSUM_FILE}'" ';'
	dd if=/dev/zero of="${DESTDIR_PATH}/wiki.hst" bs=67584 count=1
	dd if=/dev/zero of="${DESTDIR_PATH}/wiki.pas" bs=40 count=1
	echo "wiki_id=1" > "${DESTDIR_PATH}/wiki.ini"


.PHONY: misc-files-install
misc-files-install: validate-destdir
	${RM} "${MISC_FILES}"/*~
	${RM} "${LICENSES}"/*~
	cp -p "${MISC_FILES}"/* "${DESTDIR_PATH}"/
	cp -p "${LICENSES}"/* "${DESTDIR_PATH}"/

.PHONY: validate-destdir
validate-destdir:
	@if [ ! -d "${DESTDIR_PATH}" ] ; then echo DESTDIR: "'"${DESTDIR_PATH}"'" is not a directory ; exit 1; fi
	@if [ ! -d "${DESTDIR_PATH}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX}" ] ; then echo DESTDIR: "'"${DESTDIR_PATH}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX}"'" is not a directory ; exit 1; fi


# Main program
# ============

# default: progress bar = off
PROGRESS_BAR ?= NO
$(call STD_RULE, mahatma, ${SAMO_LIB}/mahatma, mini-libc fatfs drivers, INSTALL, PROGRESS_BAR="${PROGRESS_BAR}")


# Libraries
# =========

$(call STD_RULE, mini-libc, ${SAMO_LIB}/mini-libc, toolchain)
$(call STD_RULE, fatfs, ${SAMO_LIB}/fatfs, mini-libc drivers)
$(call STD_RULE, drivers, ${SAMO_LIB}/drivers, mini-libc)


# GCC and Binutils toolchain
# ==========================

BINUTILS_FILE = ${DOWNLOAD_DIR}/${BINUTILS_PACKAGE}
BINUTILS_SUM = ${SUM_DIR}/${BINUTILS_PACKAGE}.SHA256

GCC_FILE = ${DOWNLOAD_DIR}/${GCC_PACKAGE}
GCC_SUM = ${SUM_DIR}/${GCC_PACKAGE}.SHA256

GET_FILE = $(eval $(call GET_FILE1,$(strip ${1}),$(strip ${2}),$(strip ${3}),$(strip ${4})))

define GET_FILE1

${1}:
	@mkdir -p "$${DOWNLOAD_DIR}"
	@if ! ([ -f "${2}" ] && cd "$${DOWNLOAD_DIR}" && sha256sum --check "${3}") ; \
	then \
	  ${RM} "${2}" ; \
	  wget -c -O "${2}" "${4}"; \
	else \
	  echo Already downloaded: ${2} ; \
	fi
	@${TOUCH} "$$@"

endef

$(call GET_FILE,binutils-download,${BINUTILS_FILE},${BINUTILS_SUM},${BINUTILS_URL})
$(call GET_FILE,gcc-download,${GCC_FILE},${GCC_SUM},${GCC_URL})

binutils-patch: binutils-download
	mkdir -p ${HOST_TOOLS}/toolchain-install
	${RM} -r "${HOST_TOOLS}/binutils-${BINUTILS_PACKAGE}"
	tar -xvzf "${BINUTILS_FILE}" -C ${HOST_TOOLS}
	cd "${HOST_TOOLS}/binutils-${BINUTILS_VERSION}" && \
	for p in ../toolchain-patches/*-binutils-*.patch ; \
	do \
	   patch -p1 < "$${p}" ; \
	done
	${TOUCH} "$@"

binutils: binutils-patch
	cd "${HOST_TOOLS}/binutils-${BINUTILS_VERSION}" && \
	mkdir -p build && \
	cd build  && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" ../configure --prefix "${HOST_TOOLS}/toolchain-install" --target=c33-epson-elf && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" ${MAKE} && \
	${MAKE} install
	${TOUCH} "$@"

gcc-patch: gcc-download
	mkdir -p ${HOST_TOOLS}/toolchain-install
	tar -xvzf "${GCC_FILE}" -C ${HOST_TOOLS}
	cd "${HOST_TOOLS}/gcc-${GCC_VERSION}" && \
	for p in ../toolchain-patches/*-gcc-*.patch ; \
	do \
	   patch -p1 < "$${p}" ; \
	done
	${TOUCH} "$@"

gcc: binutils gcc-patch
	export PATH="${HOST_TOOLS}/toolchain-install/bin:${PATH}" && \
	cd "${HOST_TOOLS}/gcc-${GCC_VERSION}" && \
	mkdir -p build && \
	cd build && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" ../configure --prefix "${HOST_TOOLS}/toolchain-install" --target=c33-epson-elf --enable-languages=c && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" ${MAKE} && \
	${MAKE} install
	${TOUCH} "$@"


.PHONY: toolchain
toolchain: toolchain-requires gcc

.PHONY: toolchain-requires
toolchain-requires:
	true $(call REQUIRED_BINARY, patch, patch)
	true $(call REQUIRED_BINARY, gcc, gcc)
	true $(call REQUIRED_BINARY, lex, flex)
	true $(call REQUIRED_BINARY, bison, bison)

REQUIRES_TARGETS += toolchain-requires


# QT simulator
# ============


.PHONY: qt4-simulator
qt4-simulator: qt4-simulator-requires
	cd ${HOST_TOOLS}/qt4-simulator && qmake && ${MAKE}

.PHONY: qt4-simulator-requires
qt4-simulator-requires:
	true $(call REQUIRED_BINARY, g++, g++)
	true $(call REQUIRED_BINARY, qmake, qt4-qmake libqt4-dev)

REQUIRES_TARGETS += qt4-simulator-requires

.PHONY: qt4-simulator-clean
qt4-simulator-clean:
	${MAKE} -C "${HOST_TOOLS}/qt4-simulator" distclean || true

CLEAN_TARGETS += qt4-simulator-clean


.PHONY: sim4
sim4: qt4-simulator validate-destdir
	cd "${DESTDIR}" && ${HOST_TOOLS}/qt4-simulator/bin/wikisim ${ARTICLE}

.PHONY: sim4d
sim4d: qt4-simulator validate-destdir
	cd "${DESTDIR}" && gdb --args ${HOST_TOOLS}/qt4-simulator/bin/wikisim ${ARTICLE}

# the console simulator is presently broken
#$(call STD_RULE, console-simulator, ${HOST_TOOLS}/console-simulator)


# Font processing
# ===============

$(call STD_RULE, pcf2bmf, ${HOST_TOOLS}/pcf2bmf)
$(call STD_RULE, fonts, ${HOST_TOOLS}/fonts, pcf2bmf, INSTALL)


# Compression interface
# =====================

# in case the platform lacks the python-lzma package
# (only present in  ?Ubuntu >= lucid 10.04)
# then there is a local verion of the older PyLZMA module
# that can be installed instead

.PHONY: local-pylzma-install
local-pylzma-install:
	${MAKE} -C "${HOST_TOOLS}/offline-renderer" pylzma


# Build the database from wiki XML files
# ======================================

$(call STD_BASE, offline-renderer, ${HOST_TOOLS}/offline-renderer)

XML_FILES_PATH = $(realpath ${XML_FILES})
RENDER_BLOCK ?= 0

# rendering compression parameters
# images are the output of the math turning it to NO will show TeX from 'alt'
ENABLE_LANGUAGES_LINKS ?= YES
ENABLE_IMAGES ?= YES
ARTICLES_PER_BLOCK ?= 1
ARTICLE_BLOCK_SIZE ?= 262144
MAX_ARTICLE_LENGTH ?= UNLIMITED


# erase the working directories for the current language
.PHONY: cleandirs
cleandirs:
	@if [ -z "${DESTDIR_PATH}" ] ; then echo missing DESTDIR ; exit 1 ; fi
	@if [ -z "${WORKDIR_PATH}" ] ; then echo missing WORKDIR ; exit 1 ; fi
	@if [ -z "${TEMPDIR_PATH}" ] ; then echo missing TEMPDIR ; exit 1 ; fi
	${RM} -r "${DESTDIR_PATH}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX}"
	${RM} -r "${TEMPDIR_PATH}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX}"
	${RM} -r "${WORKDIR_PATH}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX}"

# only create dirs (does not erase them)
.PHONY: createdirs
createdirs:
	@if [ -z "${DESTDIR_PATH}" ] ; then echo missing DESTDIR ; exit 1 ; fi
	@if [ -z "${WORKDIR_PATH}" ] ; then echo missing WORKDIR ; exit 1 ; fi
	@if [ -z "${TEMPDIR_PATH}" ] ; then echo missing TEMPDIR ; exit 1 ; fi
	${MKDIR} "${DESTDIR_PATH}"
	${MKDIR} "${DESTDIR_PATH}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX}"
	${MKDIR} "${WORKDIR_PATH}"
	${MKDIR} "${WORKDIR_PATH}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX}"
	${MKDIR} "${TEMPDIR_PATH}"
	${MKDIR} "${TEMPDIR_PATH}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX}"


.PHONY: index
index: validate-destdir
	${MAKE} -C "${HOST_TOOLS}/offline-renderer" index \
		WIKI_LANGUAGE="${WIKI_LANGUAGE}" WIKI_FILE_PREFIX="${WIKI_FILE_PREFIX}" \
		WIKI_DIR_SUFFIX="${WIKI_DIR_SUFFIX}" \
		XML_FILES="${XML_FILES_PATH}" RENDER_BLOCK="${RENDER_BLOCK}" \
		WIKI_VERSION="${WIKI_VERSION}" \
		TEMPDIR="${TEMPDIR_PATH}" \
		WORKDIR="${WORKDIR_PATH}" DESTDIR="${DESTDIR_PATH}"

.PHONY: parse
parse: validate-destdir
	${MAKE} -C "${HOST_TOOLS}/offline-renderer" parse \
		WIKI_LANGUAGE="${WIKI_LANGUAGE}" WIKI_FILE_PREFIX="${WIKI_FILE_PREFIX}" \
		WIKI_DIR_SUFFIX="${WIKI_DIR_SUFFIX}" \
		XML_FILES="${XML_FILES_PATH}" RENDER_BLOCK="${RENDER_BLOCK}" \
		WIKI_VERSION="${WIKI_VERSION}" \
		TEMPDIR="${TEMPDIR_PATH}" \
		WORKDIR="${WORKDIR_PATH}" DESTDIR="${DESTDIR_PATH}"

.PHONY: render
render: fonts validate-destdir
	${MAKE} -C "${HOST_TOOLS}/offline-renderer" render \
		WIKI_LANGUAGE="${WIKI_LANGUAGE}" WIKI_FILE_PREFIX="${WIKI_FILE_PREFIX}" \
		WIKI_DIR_SUFFIX="${WIKI_DIR_SUFFIX}" \
		XML_FILES="${XML_FILES_PATH}" RENDER_BLOCK="${RENDER_BLOCK}" \
		ENABLE_LANGUAGES_LINKS="${ENABLE_LANGUAGES_LINKS}" \
		ENABLE_IMAGES="${ENABLE_IMAGES}" \
		ARTICLES_PER_BLOCK="${ARTICLES_PER_BLOCK}" \
		ARTICLE_BLOCK_SIZE="${ARTICLE_BLOCK_SIZE}" \
		MAX_ARTICLE_LENGTH="${MAX_ARTICLE_LENGTH}" \
		WIKI_VERSION="${WIKI_VERSION}" \
		TEMPDIR="${TEMPDIR_PATH}" \
		WORKDIR="${WORKDIR_PATH}" DESTDIR="${DESTDIR_PATH}"

.PHONY: combine
combine: validate-destdir
	${MAKE} -C "${HOST_TOOLS}/offline-renderer" combine \
		WIKI_LANGUAGE="${WIKI_LANGUAGE}" WIKI_FILE_PREFIX="${WIKI_FILE_PREFIX}" \
		WIKI_DIR_SUFFIX="${WIKI_DIR_SUFFIX}" \
		XML_FILES="${XML_FILES_PATH}" RENDER_BLOCK="${RENDER_BLOCK}" \
		WIKI_VERSION="${WIKI_VERSION}" \
		TEMPDIR="${TEMPDIR_PATH}" \
		WORKDIR="${WORKDIR_PATH}" DESTDIR="${DESTDIR_PATH}"


# run all stages (for testing small XML sample files)
.PHONY: iprc
iprc: index parse render combine


# Build database using multiple machines
# ======================================

ifneq (,$(strip ${WORKDIR_PATH}))
ifneq (,$(strip ${DESTDIR_PATH}))

INDEX_STAMP := ${WORKDIR_PATH}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX}/stamp-index
PARSE_STAMP := ${WORKDIR_PATH}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX}/stamp-parse
RENDER_STAMP := ${WORKDIR_PATH}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX}/stamp-render


# ---------------------------------------------------------------------------------------
# Get the number of articles from the indexer and compute how many articles per instance
# ---------------------------------------------------------------------------------------

MACHINE_COUNT ?= 9
PARALLEL_BUILD ?= 3

COUNTS_FILE = ${WORKDIR_PATH}/${WIKI_LANGUAGE}${WIKI_DIR_SUFFIX}/counts.text

TOTAL_ARTICLES = $(shell awk '/^Articles:/{ print $$2 }' "${COUNTS_FILE}" 2>/dev/null || echo 0)
TOTAL_INSTANCES := $(shell expr ${MACHINE_COUNT} '*' ${PARALLEL_BUILD})
# 64 dat files is the maximum allowed
CHECK := $(shell if [ ${TOTAL_INSTANCES} -gt 64 ]; then echo 0; else echo 1; fi )
ifeq ($(strip ${CHECK}),0)
  $(error Too many machines or processes being used. Maximum is 64 total instances)
endif
MAX_BLOCK := $(shell expr ${TOTAL_INSTANCES} - 1)

ARTICLES_PER_INSTANCE = $(shell expr ${TOTAL_ARTICLES} / ${TOTAL_INSTANCES})

ITEMS = $(shell i=0; while [ $${i} -lt ${TOTAL_INSTANCES} ]; do echo $${i}; i=$$(($${i} + 1)); done)


# check that the counts are correct to render all articles
.PHONY: print-render-info
print-render-info:
	@echo WORKDIR_PATH = ${WORKDIR_PATH}
	@echo DESTDIR_PATH = ${DESTDIR_PATH}
	@echo TEMPDIR_PATH = ${TEMPDIR_PATH}
	@echo XML_FILES_PATH = ${XML_FILES_PATH}
	@echo TOTAL_ARTICLES = ${TOTAL_ARTICLES}
	@echo MACHINE_COUNT = ${MACHINE_COUNT}
	@echo PARALLEL_BUILD = ${PARALLEL_BUILD}
	@echo TOTAL_INSTANCES = ${TOTAL_INSTANCES}
	@echo ARTICLES_PER_INSTANCE = ${ARTICLES_PER_INSTANCE}
	@echo ITEMS = ${ITEMS}
	@echo files = 0 .. ${MAX_BLOCK}


# index

${INDEX_STAMP}:
	${RM} "$@"
	${MAKE} -C "${HOST_TOOLS}/offline-renderer" index \
		XML_FILES="${XML_FILES_PATH}" RENDER_BLOCK="${RENDER_BLOCK}" \
		WORKDIR="${WORKDIR_PATH}" DESTDIR="${DESTDIR_PATH}"
	${TOUCH} "$@"

.PHONY: index-clean
index-clean:
	${RM} "${INDEX_STAMP}" "${COUNTS_FILE}"
	${RM} "${WORKDIR_PATH}"/*.db
	${RM} "${WORKDIR_PATH}"/*.db.*


# create the individual threads

# makeblock 0..n
MAKE_BLOCK = $(eval $(call MAKE_BLOCK1,$(strip ${1})))

define MAKE_BLOCK1

.PHONY: parse${1}
parse${1}: ${PARSE_STAMP}${1}

.PHONY: render${1}
render${1}: ${RENDER_STAMP}${1}


START_${1} = $$(shell expr ${1} '*' $${ARTICLES_PER_INSTANCE} + 1)
COUNT_${1} = $$(shell if [ "${1}" -ge $$$$(($${TOTAL_INSTANCES} - 1)) ] ; then echo all; else echo $${ARTICLES_PER_INSTANCE}; fi)

${PARSE_STAMP}${1}: ${INDEX_STAMP}
	${RM} "$$@"
	$${MAKE} RENDER_BLOCK='${1}' START='$${START_${1}}' COUNT='$${COUNT_${1}}' parse
	${TOUCH} "$$@"

${RENDER_STAMP}${1}: ${PARSE_STAMP}${1}
	${RM} "$$@"
	$${MAKE} RENDER_BLOCK='${1}' render
	${TOUCH} "$$@"

.PHONY: stamp-clean${1}
stamp-clean${1}: stamp-parse-clean${1} stamp-render-clean${1}

.PHONY: stamp-parse-clean${1}
stamp-parse-clean${1}:
	${RM} "${PARSE_STAMP}${1}"

# this is to reuse the existing parsed html after a re-index
# (but only if the stamps already exist)
.PHONY: stamp-parse-touch${1}
stamp-parse-touch${1}:
	[ -e "${PARSE_STAMP}${1}" ] && ${TOUCH} "${PARSE_STAMP}${1}"

.PHONY: stamp-render-clean${1}
stamp-render-clean${1}:
	${RM} "${RENDER_STAMP}${1}"

endef


$(foreach i,${ITEMS},$(call MAKE_BLOCK,${i}))


# Per machine rules

MAKE_FARM = $(eval $(call MAKE_FARM1,$(strip ${1}),$(strip ${2}),$(strip ${3})))

define MAKE_FARM1

.PHONY: farm${1}-parse
farm${1}-parse: $$(foreach i,${2},parse$$(strip $${i}))

.PHONY: farm${1}-render
farm${1}-render: $$(foreach i,${2},render$$(strip $${i}))

.PHONY: farm${1}-clean
farm${1}-clean: farm${1}-parse-clean farm${1}-render-clean

.PHONY: farm${1}-parse-clean
farm${1}-parse-clean: $$(foreach i,${2},stamp-parse-clean$$(strip $${i}))

.PHONY: farm${1}-parse-touch
farm${1}-parse-touch: $$(foreach i,${2},stamp-parse-touch$$(strip $${i}))

.PHONY: farm${1}-render-clean
farm${1}-render-clean: $$(foreach i,${2},stamp-render-clean$$(strip $${i}))

.PHONY: farm${1}
farm${1}: farm${1}-parse farm${1}-render

endef

.PHONY: farm-index
farm-index: ${INDEX_STAMP}


MAKE_MACHINE = $(eval $(call MAKE_MACHINE1,$(strip ${1}),$(strip ${2}),$(strip ${3})))

define MAKE_MACHINE1

$(call MAKE_FARM,${1}, $(shell j=$$((${1} - 1)); while [ $${j} -lt ${TOTAL_INSTANCES} ]; do echo $${j}; j=$$(($${j} + ${2})); done))

endef

MACHINE_LIST  := $(shell i=1; while [ $${i} -le ${MACHINE_COUNT} ]; do echo $${i}; i=$$(($${i} + 1)); done)


$(foreach i,${MACHINE_LIST},$(call MAKE_MACHINE,${i},${MACHINE_COUNT}))


# end of WORKDIR/DESTDIR check above
endif
endif


# Download the latest translations
# ================================

ALL_LANGUAGES := af ar br cs da de en es fi fr gl gsw hu ia ja mk ml nl no pms pt pt-br ru te tr
LICENSE_DIR = XML-Licenses


# These items reference wiki articles since the process needs XML
# data files, the macro below will export the xml and download it
# directly from the sites.
#
# Any items left blank will revert to English version
#
# Note: the ones that link to deed.XX are html so cannot be used
#       with the present program (these will just use English).

TERMS_da   := Wikipedia:Ophavsret
#LICENSE_da := http://creativecommons.org/licenses/by-sa/3.0/deed.da

TERMS_de   := Nutzungsbedingungen
LICENSE_de := Wikipedia:Lizenzbestimmungen_Commons_Attribution-ShareAlike_3.0_Unported

TERMS_en   := Terms_of_Use
LICENSE_en := Wikipedia:Text_of_Creative_Commons_Attribution-ShareAlike_3.0_Unported_License

TERMS_es   := Términos_de_Uso
#LICENSE_es := http://creativecommons.org/licenses/by-sa/3.0/deed.es

TERMS_fi   := Käyttöehdot
LICENSE_fi := Wikipedia:Creative_Commons_Attribution-Share_Alike_3.0_Unported_-lisenssiehdot

TERMS_fr   := Conditions_d\'utilisation
#LICENSE_fr := http://creativecommons.org/licenses/by-sa/3.0/deed.fr

TERMS_ja   := 利用規約
LICENSE_ja := Wikipedia:Text_of_Creative_Commons_Attribution-ShareAlike_3.0_Unported_License

TERMS_nl   := Gebruiksvoorwaarden
#LICENSE_nl := http://creativecommons.org/licenses/by-sa/3.0/deed.nl

TERMS_pt   := Condições_de_Uso
#LICENSE_pt := http://creativecommons.org/licenses/by-sa/3.0/deed.pt


WIKIMEDIA_EXPORT := http://wikimediafoundation.org/wiki/Special:Export
LICENSE_EXTRA_SUFFIX := ?curonly=1&templates=1&wpDownload=1
TERMS_EXTRA_SUFFIX := ?curonly=1&wpDownload=1

# macro to retrieve files from various web sites
define GET_FILES
  mkdir -p "${LICENSE_DIR}/${1}" ; \
  [ -n "${2}" ] && \
    wget --output-document="-" "${WIKIMEDIA_EXPORT}/${2}${TERMS_EXTRA_SUFFIX}" \
    | sed 's/[[:space:]]*$$//' > "${LICENSE_DIR}/${1}/terms.xml" ; \
  [ -n "${3}" ] && \
    wget --output-document="-" "http://${1}.wikipedia.org/wiki/Special:Export/${3}${LICENSE_EXTRA_SUFFIX}" \
    | sed 's/[[:space:]]*$$//;s/<title>Wikipedia:/<title>Wikipedia - /' > "${LICENSE_DIR}/${1}/license.xml" ; \
  true ;
endef


.PHONY: fetch-nls
fetch-nls: $(foreach lang,${ALL_LANGUAGES},fetch-nls-${lang})

# fetch one language e.g.  fetch-nls-en
fetch-nls-%:
	echo fetching nls files for: $*
	$(call GET_FILES,$*,${TERMS_$*},${LICENSE_$*})

# install nls file
.PHONY: nls-install
nls-install: validate-destdir
	@find "${DESTDIR}" -type d | \
	  ( while read dir ; \
	    do \
	      d=$$(basename "$${dir}") ; \
	      for suffix in books dict pedia quote ; \
	      do \
	        language="$${d%$${suffix}}" ; \
	        if [ X"$${language}" != X"$${d}" ] ; \
	        then \
	          src="${LICENSE_DIR}/$${d%$${suffix}}/wiki.nls" ; \
	          native_name=$$(${SCRIPTS}/nls-print "$${language}") ; \
	          [ -f "$${src}" ] || src="${LICENSE_DIR}/en/wiki.nls" ; \
	          ${SCRIPTS}/nls-installer --prefix="${DESTDIR_PATH}" \
                                           --language="$${language}" \
                                           --suffix="$${suffix}" \
                                           --output="wiki.nls" \
                                           --nls="$${src}" \
                                           --change="$${native_name}" \
                                           --verbose ; \
	          break ; \
	        fi ; \
	      done \
	    done \
	  )


# Download the latest Mediawiki dump
# ==================================

.PHONY: getwikidump
getwikidump:
	wget http://download.wikimedia.org/enwiki/latest/enwiki-latest-pages-articles.xml.bz2


# Forth interpreter
# =================

$(call STD_RULE, forth, ${SAMO_LIB}/forth, gcc mini-libc fatfs drivers, INSTALL)


# FLASH programmer that runs on the device
# ========================================

$(call STD_RULE, flash, ${SAMO_LIB}/flash, gcc mini-libc fatfs drivers, INSTALL)


# Grifo small kernel
# ==================

$(call STD_RULE, grifo, ${SAMO_LIB}/grifo, gcc mini-libc fatfs, INSTALL)


# Master boot record
# ==================

define FindTTY
for i in USBconsole ttyUSB2 ttyUSB1 ttyUSB0;
do
  d="/dev/$${i}";
  if [ -e "$${d}" ];
  then
    echo -n $${d};
    exit 0;
  fi;
done;
echo -n /dev/TTY-NOT-FOUND;
exit 1;
endef

define FindAUX
for i in USBjtag;
do
  d="/dev/$${i}";
  if [ -e "$${d}" ];
  then
    echo -n $${d};
    exit 0;
  fi;
done;
echo -n /dev/TTY-NOT-FOUND;
exit 1;
endef

BOOTLOADER_TTY ?= $(shell ${FindTTY})
BOOTLOADER_AUX ?= $(shell ${FindAUX})
SERIAL_NUMBER ?= No Serial Number
FLASH_UPDATE ?= NO


.PHONY: print-mbr-tty
print-mbr-tty:
	@echo BOOTLOADER_TTY = "${BOOTLOADER_TTY}"
	@echo BOOTLOADER_AUX = "${BOOTLOADER_AUX}"

$(call STD_RULE, mbr, ${SAMO_LIB}/mbr, gcc fatfs, INSTALL)
$(call STD_RULE, jackknife, ${HOST_TOOLS}/jackknife)
$(call STD_RULE, flash07, ${HOST_TOOLS}/flash07)

.PHONY: mbr-rs232
mbr-rs232: gcc fatfs
	${MAKE} -C "${SAMO_LIB}/mbr" mbr-rs232

.PHONY: flash-mbr
flash-mbr: mbr jackknife flash07
	${MAKE} -C "${SAMO_LIB}/mbr" BOOTLOADER_TTY="${BOOTLOADER_TTY}" BOOTLOADER_AUX="${BOOTLOADER_AUX}" SERIAL_NUMBER="${SERIAL_NUMBER}" FLASH_UPDATE="${FLASH_UPDATE}" $@

.PHONY: flash-test-jig
flash-test-jig: mbr jackknife flash07
	${MAKE} -C "${SAMO_LIB}/mbr" FLASH_TEST_JIG=YES BOOTLOADER_TTY="${BOOTLOADER_TTY}" BOOTLOADER_AUX="${BOOTLOADER_AUX}" SERIAL_NUMBER="${SERIAL_NUMBER}" flash-mbr


# final list of all items to build
# ================================

.PHONY: all-targets
all-targets: ${ALL_TARGETS}


# list required packages
# ======================

.PHONY: requirements
requirements: ${REQUIRES_TARGETS}


# Clean up generated files
# ========================

.PHONY: complete-clean
complete-clean: clean clean-toolchain
	${RM} binutils-download gcc-download

.PHONY: clean
clean: ${CLEAN_TARGETS}
	${RM} stamp-r-*

.PHONY: clean-toolchain
clean-toolchain:
	${RM} -r "${HOST_TOOLS}/toolchain-install"
	${RM} -r "${HOST_TOOLS}/gcc-${GCC_VERSION}"
	${RM} -r "${HOST_TOOLS}/binutils-${BINUTILS_VERSION}"
	${RM} binutils-patch binutils
	${RM} gcc-patch gcc


# Update the Makefiles
# ====================

# Change the method of including definitions by
# copying part of Mk/definitions.mk into each Makefile
# if it requires it.

.PHONY: update-makefiles
update-makefiles:
	@${SCRIPTS}/UpdateMakefiles --verbose \
	  --source="${MK_DIR}/definitions.mk" \
	  $(shell find "${ROOT_DIR}" -name Makefile)


# Print information about some targets
# ====================================

.PHONY: help
help:
	@echo
	@echo 'Some of the more useful targets:'
	@echo
	@echo '  all                   - compile all the source'
	@echo '  install               - install forth, mahatma, fonts in DESTDIR'
	@echo '  index                 - convert XML_FILES to index files in DESTDIR'
	@echo '  parse                 - parse XML_FILES into one HTML file in WORKDIR'
	@echo '  render                - render HTML in WORKDIR into one big data file in DESTDIR'
	@echo '  combine               - combine temporary indices to one file in DESTDIR'
	@echo '  iprc                  - same as: index parse render combine'
	@echo '  farm<1..N>            - parse/render XML_FILES into 3 data files in DESTDIR (use -j3)'
	@echo '  farm<1..N>-parse      - parse XML_FILES into 3 HTML files in WORKDIR (use -j3)'
	@echo '  farm<1..N>-render     - render WORKDIR HTML files into 3 data files in DESTDIR (use -j3)'
	@echo '  farm<1..N>-clean      - remove stamp files to repeat process'
	@echo '  mbr                   - compile bootloader'
	@echo '  mbr-install           - install flash.rom in DESTDIR'
	@echo '  mahatma               - compile kernel'
	@echo '  mahatma-install       - install mahatma as kernel in DESTDIR'
	@echo '  forth                 - compile forth'
	@echo '  forth-install         - install forth files in DESTDIR'
	@echo '  flash                 - compile flash (programs flash.rom from SD Card'
	@echo '  flash-install         - install flash programmer in DESTDIR'
	@echo '  fonts                 - compile fonts'
	@echo '  fonts-install         - install font files in DESTDIR'
	@echo '  gcc                   - compile gcc toolchain'
	@echo '  flash-mbr             - flash bootloader to the E07 board'
	@echo '  qt4-simulator         - compile the Qt4 simulator'
	@echo '  sim4  sim4d           - use the data file in DESTDIR and run the qt4-simulator (d => gdb)'
	@echo '  console-simulator     - compile the console simulator'
	@echo '  requirements          - detect missing packages'
	@echo '  clean                 - clean all programs and object files except the toochain'
	@echo '  clean-toolchain       - clean just the toochain'
	@echo '  cleandirs             - clean work/temp/image for current language'
	@echo '  createdirs            - create work/temp/image for current language'
	@echo '  jig-install           - copy flash program and image; forth and programs to SD Card'
	@echo '  p33                   - terminal emulator (console debugging)'
	@echo '  fetch-nls             - Fetch nls, texts and license files from web'
	@echo


.PHONY: testhelp
testhelp:
	${MAKE} --print-data-base --question |	\
	awk '/^[^.%][-A-Za-z0-9_]*:/		\
		{ print substr($$1, 1, length($$1)-1) }' | 	\
	sort |	\
	pr --omit-pagination --width=80 --columns=1

# Display a make variable
# =======================

# use this like: make print-MAKE
# which shows the value of ${MAKE}

print-%:
	@echo $* is $($*)


# Run teminal emulator script
# ==========================

.PHONY: p33
p33:
	${SCRIPTS}/p33
