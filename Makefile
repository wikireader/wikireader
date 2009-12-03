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

# ----- Toolchain configuration data --------------------------------------

GCC_VERSION = 3.3.2
GCC_PACKAGE = gcc-${GCC_VERSION}.tar.gz
GCC_URL = ftp://ftp.gnu.org/gnu/gcc/${GCC_PACKAGE}

BINUTILS_VERSION = 2.10.1
BINUTILS_PACKAGE = binutils-${BINUTILS_VERSION}.tar.gz
BINUTILS_URL= ftp://ftp.gnu.org/gnu/binutils/${BINUTILS_PACKAGE}

SAMO_LIB := $(shell readlink -m ./samo-lib)
HOST_TOOLS := $(shell readlink -m ./host-tools)
LICENSES := $(shell readlink -m ./Licenses)
MISC_FILES := ${SAMO_LIB}/misc-files

DOWNLOAD_DIR = ${HOST_TOOLS}/toolchain-download
SUM_DIR = ${HOST_TOOLS}/toolchain-sums

export PATH := ${HOST_TOOLS}/toolchain-install/bin:${PATH}

CONFIG_FILE := "${SAMO_LIB}/include/config.h"
CONFIG_FILE_DEFAULT := "${SAMO_LIB}/include/config.h-default"
CONFIG_FILE_EXISTS := $(shell [ -f ${CONFIG_FILE} ] && echo 1)

ifeq (${CONFIG_FILE_EXISTS},)
$(shell cp ${CONFIG_FILE_DEFAULT} ${CONFIG_FILE})
$(error edit ${CONFIG_FILE} file and re-run make)
endif

# ----- configuration data --------------------------------------

ALL_TARGETS =
ALL_TARGETS += mbr
ALL_TARGETS += jackknife
ALL_TARGETS += forth
ALL_TARGETS += flash
ALL_TARGETS += mahatma
ALL_TARGETS += qt4-simulator
ALL_TARGETS += hash-gen
ALL_TARGETS += pcf2bmf
ALL_TARGETS += fonts

.PHONY: all
all:    ${ALL_TARGETS}


# ----- installation ------------------------------------------

DESTDIR_PATH := $(shell readlink -m "${DESTDIR}")
WORKDIR_PATH := $(shell readlink -m "${WORKDIR}")
VERSION_TAG ?= $(shell date '+%Y%m%d%H%M')
VERSION_FILE := ${DESTDIR_PATH}/version.txt
SHA_LEVEL := 256
CHECKSUM_FILE := ${DESTDIR_PATH}/sha${SHA_LEVEL}.txt

.PHONY: jig-install
jig-install: validate-destdir forth-install flash-install mbr-install

.PHONY: install
install: validate-destdir forth-install mahatma-install fonts-install misc-files-install version

.PHONY: version
version: validate-destdir
	@if [ -z "${VERSION_TAG}" ] ; then echo VERSION_TAG: "'"${VERSION_TAG}"'" is not valid ; exit 1; fi
	${RM} "${VERSION_FILE}" "${CHECKSUM_FILE}" "${DESTDIR_PATH}"/*.idx-tmp "${DESTDIR_PATH}"/*~
	echo VERSION: ${VERSION_TAG} >> "${VERSION_FILE}"
	cd "${DESTDIR_PATH}" && sha${SHA_LEVEL}sum * >> "${CHECKSUM_FILE}"


.PHONY: misc-files-install
misc-files-install: validate-destdir
	${RM} "${MISC_FILES}"/*~
	${RM} "${LICENSES}"/*~
	cp -p "${MISC_FILES}"/* "${DESTDIR_PATH}"/
	cp -p "${LICENSES}"/* "${DESTDIR_PATH}"/

.PHONY: validate-destdir
validate-destdir:
	@if [ ! -d "${DESTDIR_PATH}" ] ; then echo DESTDIR: "'"${DESTDIR_PATH}"'" is not a directory ; exit 1; fi


# ----- main program ------------------------------------------

.PHONY: mahatma
mahatma: mini-libc fatfs
	${MAKE} -C ${SAMO_LIB}/mahatma

.PHONY: mahatma-install
mahatma-install: mahatma validate-destdir
	${MAKE} -C ${SAMO_LIB}/mahatma install DESTDIR="${DESTDIR_PATH}"


# ----- lib stuff   -------------------------------------------
.PHONY:mini-libc
mini-libc: gcc
	${MAKE} -C ${SAMO_LIB}/mini-libc/

.PHONY: fatfs
fatfs: mini-libc drivers
	${MAKE} -C ${SAMO_LIB}/fatfs/

.PHONY: drivers
drivers: mini-libc
	${MAKE} -C ${SAMO_LIB}/drivers/

# ----- toolchain stuff  --------------------------------------

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
	@touch "$$@"

endef

$(call GET_FILE,binutils-download,${BINUTILS_FILE},${BINUTILS_SUM},${BINUTILS_URL})
$(call GET_FILE,gcc-download,${GCC_FILE},${GCC_SUM},${GCC_URL})

binutils-patch: binutils-download
	mkdir -p ${HOST_TOOLS}/toolchain-install
	rm -rf "${HOST_TOOLS}/binutils-${BINUTILS_PACKAGE}"
	tar -xvzf "${BINUTILS_FILE}" -C ${HOST_TOOLS}
	cd ${HOST_TOOLS} && \
	cd "binutils-${BINUTILS_VERSION}" && \
	for p in ../toolchain-patches/*-binutils-*.patch ; \
	do \
	   patch -p1 < "$${p}" ; \
	done
	touch "$@"

binutils: binutils-patch
	cd ${HOST_TOOLS} && \
	cd "binutils-${BINUTILS_VERSION}" && \
	mkdir -p build && \
	cd build  && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" ../configure --prefix "${HOST_TOOLS}/toolchain-install" --target=c33-epson-elf && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" ${MAKE} && \
	${MAKE} install
	touch "$@"

gcc-patch: gcc-download
	mkdir -p ${HOST_TOOLS}/toolchain-install
	tar -xvzf "${GCC_FILE}" -C ${HOST_TOOLS}
	cd ${HOST_TOOLS} && \
	cd "gcc-${GCC_VERSION}" && \
	for p in ../toolchain-patches/*-gcc-*.patch ; \
	do \
	   patch -p1 < "$${p}" ; \
	done
	touch "$@"

gcc: binutils gcc-patch
	cd ${HOST_TOOLS} && \
	export PATH="${HOST_TOOLS}/toolchain-install/bin:${PATH}" && \
	cd "gcc-${GCC_VERSION}" && \
	mkdir -p build && \
	cd build && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" ../configure --prefix "${HOST_TOOLS}/toolchain-install" --target=c33-epson-elf --enable-languages=c && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" ${MAKE} && \
	${MAKE} install
	touch "$@"

.PHONY: qt4-simulator
qt4-simulator: mahatma
	cd host-tools/qt4-simulator && qmake-qt4 && ${MAKE}

.PHONY: console-simulator
console-simulator:
	cd ${HOST_TOOLS}/console-simulator && ${MAKE}

# ----- hash-gen  --------------------------------------
.PHONY: hash-gen
hash-gen:
	cd ${HOST_TOOLS}/hash-gen && ${MAKE}


# ----- pcf2bmf  --------------------------------------
.PHONY: pcf2bmf
pcf2bmf:
	cd ${HOST_TOOLS}/pcf2bmf && ${MAKE}


# ----- fonts  --------------------------------------
.PHONY: fonts
fonts: pcf2bmf
	cd ${HOST_TOOLS}/fonts && ${MAKE}

.PHONY: fonts-install
fonts-install: fonts validate-destdir
	cd ${HOST_TOOLS}/fonts && ${MAKE} DESTDIR="${DESTDIR_PATH}" install


# ----- build the database  --------------------------------------

XML_FILES_PATH := $(foreach f,${XML_FILES},$(shell readlink -m "${f}"))
RENDER_BLOCK ?= 0

.PHONY: index
index: validate-destdir
	cd ${HOST_TOOLS}/offline-renderer && ${MAKE} index \
		XML_FILES="${XML_FILES_PATH}" RENDER_BLOCK="${RENDER_BLOCK}" \
		WORKDIR="${WORKDIR_PATH}" DESTDIR="${DESTDIR_PATH}"

.PHONY: parse
parse: validate-destdir
	cd ${HOST_TOOLS}/offline-renderer && ${MAKE} parse \
		XML_FILES="${XML_FILES_PATH}" RENDER_BLOCK="${RENDER_BLOCK}" \
		WORKDIR="${WORKDIR_PATH}" DESTDIR="${DESTDIR_PATH}"

.PHONY: render
render: fonts validate-destdir
	cd ${HOST_TOOLS}/offline-renderer && ${MAKE} render \
		XML_FILES="${XML_FILES_PATH}" RENDER_BLOCK="${RENDER_BLOCK}" \
		WORKDIR="${WORKDIR_PATH}" DESTDIR="${DESTDIR_PATH}"

.PHONY: combine
combine: validate-destdir
	cd ${HOST_TOOLS}/offline-renderer && ${MAKE} combine \
		XML_FILES="${XML_FILES_PATH}" RENDER_BLOCK="${RENDER_BLOCK}" \
		WORKDIR="${WORKDIR_PATH}" DESTDIR="${DESTDIR_PATH}"

.PHONY: hash
hash: validate-destdir hash-gen
	cd "${DESTDIR}" && ${HOST_TOOLS}/hash-gen/hash-gen \
		--pfx="${DESTDIR_PATH}/pedia.pfx" \
		--fnd="${DESTDIR_PATH}/pedia.fnd" \
		--hsh="${DESTDIR_PATH}/pedia.hsh"


stamp-r-index:
	rm -f "$@"
	cd ${HOST_TOOLS}/offline-renderer && ${MAKE} index \
		XML_FILES="${XML_FILES_PATH}" RENDER_BLOCK="${RENDER_BLOCK}" \
		WORKDIR="${WORKDIR_PATH}" DESTDIR="${DESTDIR_PATH}"
	touch "$@"

.PHONY: clean-index
clean-index:
	rm -f stamp-r-index

MAKE_BLOCK = $(eval $(call MAKE_BLOCK1,$(strip ${1}),$(strip ${2}),$(strip ${3})))

define MAKE_BLOCK1

.PHONY: parse${1}
parse${1}: stamp-r-index stamp-r-parse${1}

.PHONY: render${1} fonts parse${1}
render${1}: stamp-r-render${1}

stamp-r-parse${1}:
	rm -f "$$@"
	$${MAKE} RENDER_BLOCK=${1} START=${2} COUNT=${3} parse
	touch "$$@"

#.NOTPARALLEL: stamp-r-render${1}
stamp-r-render${1}:
	rm -f "$$@"
	$${MAKE} RENDER_BLOCK=${1} render
	touch "$$@"

.PHONY: stamp-r-clean${1}
stamp-r-clean${1}:
	rm -f stamp-r-parse${1} stamp-r-render${1}

endef

# ------------------------------------------------
# get the number of articles from the indexer
# and compute how many articles per instance
# ------------------------------------------------
TOTAL_ARTICLES := $(shell awk '/^Articles:/{ print $$2 }' work/counts.text 2>/dev/null || echo 100)
MACHINE_COUNT ?= 9
PARALLEL_BUILD ?= 3
TOTAL_INSTANCES := $(shell expr ${MACHINE_COUNT} '*' ${PARALLEL_BUILD})
ARTICLE_COUNT_K := $(shell expr '(' '(' ${TOTAL_ARTICLES} ')' /  ${TOTAL_INSTANCES} + 999 ')' / 1000)

MAX_BLOCK := $(shell expr ${TOTAL_INSTANCES} - 1)

# check that the counts are correct to render all articles
.PHONY: print-render-info
print-render-info:
	@echo TOTAL_ARTICLES  = ${TOTAL_ARTICLES}
	@echo MACHINE_COUNT   = ${MACHINE_COUNT}
	@echo PARALLEL_BUILD  = ${PARALLEL_BUILD}
	@echo TOTAL_INSTANCES = ${TOTAL_INSTANCES}
	@echo ARTICLE_COUNT_K = ${ARTICLE_COUNT_K} k articles per instance
	@echo files = 0 .. ${MAX_BLOCK}
	@t=$$((${MACHINE_COUNT} * ${PARALLEL_BUILD} * ${ARTICLE_COUNT_K} * 1000)); \
	if [ $${t} -ge ${TOTAL_ARTICLES} ]; \
	 then \
	   echo Counts calculation is OK ; \
	 else \
	   echo Counts calculation has a problem: to_build=$${t} '<' ${TOTAL_ARTICLES}; \
	 fi


# the first(0) and last(MAX_BLOCK) are special

$(call MAKE_BLOCK,0,1,$(shell expr ${ARTICLE_COUNT_K} '*' 1000 - 1))

ITEMS := $(shell i=1; while [ $${i} -lt ${MAX_BLOCK} ]; do echo $${i}; i=$$(($${i} + 1)); done)
$(foreach i,${ITEMS},$(call MAKE_BLOCK,${i},$(shell expr ${i} '*' ${ARTICLE_COUNT_K})k,${ARTICLE_COUNT_K}k))

$(call MAKE_BLOCK,${MAX_BLOCK},$(shell expr ${MAX_BLOCK} '*' ${ARTICLE_COUNT_K})k,all)


MAKE_FARM = $(eval $(call MAKE_FARM1,$(strip ${1}),$(strip ${2}),$(strip ${3})))

define MAKE_FARM1

.PHONY: farm${1}-parse
farm${1}-parse: $$(foreach i,${2},parse$$(strip $${i}))

.PHONY: farm${1}-render
farm${1}-render: $$(foreach i,${2},render$$(strip $${i}))

.PHONY: farm${1}-clean
farm${1}-clean: $$(foreach i,${2},stamp-r-clean$$(strip $${i}))

.PHONY: farm${1}
farm${1}: farm${1}-parse farm${1}-render

endef


MAKE_MACHINE = $(eval $(call MAKE_MACHINE1,$(strip ${1}),$(strip ${2}),$(strip ${3})))

define MAKE_MACHINE1

$(call MAKE_FARM,${1}, $(shell expr ${1} - 1) $(shell expr ${1} - 1 + ${2}) $(shell expr ${1} - 1 + 2 '*' ${2}))

endef

MACHINE_LIST  := $(shell i=1; while [ $${i} -le ${MACHINE_COUNT} ]; do echo $${i}; i=$$(($${i} + 1)); done)


$(foreach i,${MACHINE_LIST},$(call MAKE_MACHINE,${i},${MACHINE_COUNT}))


# ----- wiki Dump  --------------------------------------
.PHONY: getwikidump
getwikidump:
	wget http://download.wikimedia.org/enwiki/latest/enwiki-latest-pages-articles.xml.bz2


# ----- forth -----------------------------------------------
# forth interpreter

.PHONY: forth
forth:  gcc mini-libc fatfs drivers
	${MAKE} -C ${SAMO_LIB}/forth

.PHONY: forth-install
forth-install: forth
	${MAKE} -C ${SAMO_LIB}/forth install DESTDIR="${DESTDIR_PATH}"


# ----- flash -----------------------------------------------
# flash programmer that runs on the device

.PHONY: flash
flash:  gcc mini-libc fatfs drivers
	${MAKE} -C ${SAMO_LIB}/flash

.PHONY: flash-install
flash-install: flash
	${MAKE} -C ${SAMO_LIB}/flash install DESTDIR="${DESTDIR_PATH}"


# ----- grifo -----------------------------------------------
# grifo small kernel

.PHONY: grifo
grifo:  gcc mini-libc fatfs
	${MAKE} -C ${SAMO_LIB}/grifo

.PHONY: grifo-install
grifo-install: grifo
	${MAKE} -C ${SAMO_LIB}/grifo install DESTDIR="${DESTDIR_PATH}"


# ----- mbr -------------------------------------------------
# master boot record

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

.PHONY: mbr
mbr: gcc fatfs
	${MAKE} -C ${SAMO_LIB}/mbr

.PHONY: mbr-rs232
mbr-rs232: gcc fatfs
	${MAKE} -C ${SAMO_LIB}/mbr mbr-rs232

.PHONY: jackknife
jackknife:
	${MAKE} -C ${HOST_TOOLS}/jackknife

.PHONY: flash-mbr
flash-mbr: mbr jackknife
	${MAKE} -C ${SAMO_LIB}/mbr BOOTLOADER_TTY="${BOOTLOADER_TTY}" BOOTLOADER_AUX="${BOOTLOADER_AUX}" SERIAL_NUMBER="${SERIAL_NUMBER}" FLASH_UPDATE="${FLASH_UPDATE}" $@

.PHONY: flash-test-jig
flash-test-jig: mbr jackknife
	${MAKE} -C ${SAMO_LIB}/mbr FLASH_TEST_JIG=YES BOOTLOADER_TTY="${BOOTLOADER_TTY}" BOOTLOADER_AUX="${BOOTLOADER_AUX}" SERIAL_NUMBER="${SERIAL_NUMBER}" flash-mbr

.PHONY: mbr-install
mbr-install: mbr
	${MAKE} -C ${SAMO_LIB}/mbr install DESTDIR="${DESTDIR_PATH}"


# ----- clean and help --------------------------------------
.PHONY: complete-clean
complete-clean: clean clean-toolchain
	rm -f binutils-download gcc-download

.PHONY: clean
clean: clean-qt4-simulator clean-console-simulator
	${MAKE} clean -C ${SAMO_LIB}/mini-libc
	${MAKE} clean -C ${HOST_TOOLS}/jackknife
	${MAKE} clean -C ${HOST_TOOLS}/hash-gen
	${MAKE} clean -C ${HOST_TOOLS}/pcf2bmf
	${MAKE} clean -C ${HOST_TOOLS}/flash07
	${MAKE} clean -C ${HOST_TOOLS}/fonts
	${MAKE} clean -C ${HOST_TOOLS}/offline-renderer
	${MAKE} clean -C ${SAMO_LIB}/mbr
	${MAKE} clean -C ${SAMO_LIB}/drivers
	${MAKE} clean -C ${SAMO_LIB}/fatfs
	${MAKE} clean -C ${SAMO_LIB}/forth
	${MAKE} clean -C ${SAMO_LIB}/flash
	${MAKE} clean -C ${SAMO_LIB}/grifo
	${MAKE} clean -C ${SAMO_LIB}/mahatma
	${RM} stamp-r-*

.PHONY: clean-toolchain
clean-toolchain:
	rm -rf ${HOST_TOOLS}/toolchain-install
	rm -rf ${HOST_TOOLS}/gcc-${GCC_VERSION}
	rm -rf ${HOST_TOOLS}/binutils-${BINUTILS_VERSION}
	rm -f binutils-patch binutils
	rm -f gcc-patch gcc

.PHONY: clean-qt4-simulator
clean-qt4-simulator:
	(cd ${HOST_TOOLS}/qt4-simulator; ${MAKE} distclean || true)

.PHONY: clean-console-simulator
clean-console-simulator:
	${MAKE} clean -C ${HOST_TOOLS}/console-simulator

.PHONY:help
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
	@echo '  farm<1..8>            - parse/render XML_FILES into 3 data files in DESTDIR (use -j3)'
	@echo '  farm<1..8>-parse      - parse XML_FILES into 3 HTML files in WORKDIR (use -j3)'
	@echo '  farm<1..8>-render     - render WORKDIR HTML files into 3 data files in DESTDIR (use -j3)'
	@echo '  farm<1..8>-clean      - remove stamp files to repeat process'
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
	@echo '  console-simulator     - compile the console simulator'
	@echo '  clean                 - clean everything except the toochain'
	@echo '  clean-toolchain       - clean just the toochain'
	@echo '  jig-install           - copy flash program and image; forth and programs to SD Card'
	@echo '  p33                   - terminal emulator (console debugging)'
	@echo


.PHONY:testhelp
testhelp:
	${MAKE} --print-data-base --question |	\
	awk '/^[^.%][-A-Za-z0-9_]*:/		\
		{ print substr($$1, 1, length($$1)-1) }' | 	\
	sort |	\
	pr --omit-pagination --width=80 --columns=1

#use this like: make print-MAKE
print-%:
	@echo $* is $($*)

.PHONY: p33
p33:
	./${SAMO_LIB}/scripts/p33
