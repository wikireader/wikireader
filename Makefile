#
# "WikiReaderMakefile" - a Makefile for setting up Wiki Reader
#
# (C) Copyright 2008 OpenMoko, Inc.
# Author: xiangfu liu <xiangfu@openmoko.org>
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

# ----- SVN configuration data --------------------------------------

WR_SVN_SITE := wikipediardware.googlecode.com
WR_SVN_PATH := svn/trunk
WR_PATH := .

# ----- Toolchain configuration data --------------------------------------

GCC_VERSION=3.3.2
GCC_PACKAGE=gcc-$(GCC_VERSION).tar.gz
GCC_URL=ftp://ftp.gnu.org/gnu/gcc/$(GCC_PACKAGE)

BINUTILS_VERSION=2.10.1
BINUTILS_PACKAGE=binutils-$(BINUTILS_VERSION).tar.gz
BINUTILS_URL= \
  ftp://ftp.gnu.org/gnu/binutils/$(BINUTILS_PACKAGE)

DL=toolchain/dl

# ----- up is configuration data --------------------------------------

# If the user hasn't checked out sources, he/she should run 'make checkout'
# first, then run 'make'
.PHONY: all
all:    mini-libc \
	toolchain \
	bootloader \
	wikireader 

.PHONY:checkout
checkout:
	svn checkout http://${WR_SVN_SITE}/${WR_SVN_PATH}/  ${WR_PATH}

.PHONY: bootloader
bootloader:mini-libc fatfs
	( cd bootloader && \
	make )

.PHONY: wikireader
wikireader: mini-libc fatfs
	( cd jsp && \
	make -C cfg && \
	make -C wikireader && \
	cp wikireader/sample1.elf ../kernel)

# ----- lib stuff   -------------------------------------------
.PHONY:mini-libc
mini-libc:
	make -C toolchain/mini-libc/

.PHONY: fatfs
fatfs:
	make -C fatfs/

# ----- toolchain stuff  --------------------------------------
.PHONY: toolchain
toolchain:toolchain-download gcc gdb binutils

.PHONY:toolchain-download
toolchain-download: \
	$(DL)/$(GCC_PACKAGE).ok \
	  $(DL)/$(BINUTILS_PACKAGE).ok 

$(DL)/$(GCC_PACKAGE).ok:
	mkdir -p $(DL)
	wget -c -O $(DL)/$(GCC_PACKAGE) $(GCC_URL)
	touch $@

$(DL)/$(BINUTILS_PACKAGE).ok:
	mkdir -p $(DL)
	wget -c -O $(DL)/$(BINUTILS_PACKAGE) $(BINUTILS_URL)
	touch $@

.PHONY: binutils
binutils: $(DL)/$(BINUTILS_PACKAGE).ok
	mkdir -p install
	tar -xvzf $(DL)/$(BINUTILS_PACKAGE) -C toolchain/
	( cd toolchain && \
	cd binutils-$(BINUTILS_VERSION) && \
	cat ../patches/0001-binutils-EPSON-changes-to-binutils.patch | patch -p1 && \
	cat ../patches/0002-binutils-EPSON-make-it-compile-hack-for-recent-gcc.patch | patch -p1 && \
	mkdir build && \
	cd build  && \
	../configure --prefix $(PWD)/../../install --target=c33-epson-elf && \
	make && \
	make install )

.PHONY: gcc
gcc: $(DL)/$(GCC_PACKAGE).ok binutils
	mkdir -p install
	tar -xvzf $(DL)/$(GCC_PACKAGE) -C toolchain/
	( cd toolchain && \
	export PATH=$(PWD)/install/bin:\$(PATH)  && \
	cd gcc-$(GCC_VERSION) && \
	cat ../patches/0001-gcc-EPSON-modified-sources.patch | patch -p1 && \
	cat ../patches/0002-gcc-Force-that-the-assembly-of-libgcc-complies-wit.patch | patch -p1 && \
	cat ../patches/0003-gcc-Use-the-C-implementations-for-division-and-mod.patch | patch -p1 && \
	mkdir build && \
	cd build && \
	../configure --prefix $(PWD)/../../install --target=c33-epson-elf --enable-languages=c && \
	make && \
	make install )

.PHONY: gdb
gdb:


# ----- wiki Dump and Algorithm  --------------------------------------
.PHONY: getwikidump
getwikidump:
	wget http://download.wikimedia.org/enwiki/latest/enwiki-latest-pages-articles.xml.bz2

# ----- update and other things --------------------------------------
.PHONY: update
update:
	svn update

.PHONY: update-bootloader
update-bootloader: 
	( cd bootloader && svn update )

.PHONY: update-wikireader
update-wikireader: 
	( cd jsp && svn update )

.PHONY: check-makefile
check-makefile:
	( wget -O - http://${WR_SVN_SITE}/${WR_SVN_PATH}/Makefile | \
	  diff -u Makefile - )

.PHONY: flash-bootloader
flash-bootloader: bootloader
	( cd bootloader && \
		e07load/e07load wikireader.map )

# ----- clean and help --------------------------------------
.PHONY: clean
clean: 
	make clean -C bootloader
	make clean -C toolchain/mini-libc
	make clean -C fatfs
	cd jsp && make clean -C wikireader
	rm -rf toolchain/gcc-$(GCC_VERSION)
	rm -rf toolchain/binutils-$(BINUTILS_VERSION)

.PHONY:help
help:
	@echo "\n\
all:			compile all the source.\n\
setup:			get all the source we need.\n\
bootloader:		compile bootloader.\n\
wikireader:		compile wikireader then you can see kernel file you need.\n\
toolchain:		make toolchain-download gcc gdb binutils.\n\
toolchain-download:	downlaod gcc and binutils code we need.\n\
binutils: 		compile binutils.\n\
gcc:			compile gcc.\n\
mini-libc:			compile mini-libc (libc.a).\n\
update:			both update-bootloader update-wikireader.\n\
update-bootloader: 	update the bootloader source.\n\
update-wikireader: 	update the wikireader source.\n\
check-makefile:		diff the remote makefile.\n\
flash-bootloader: 	flash bootloader to you E07 board\n\
				-make sure the serial console is /dev/ttyUSB0.\n\
clean: 			clean all.\n\
				openmoko, Inc.\n "

.PHONY:testhelp
testhelp:
	make --print-data-base --question |	\
	awk '/^[^.%][-A-Za-z0-9_]*:/		\
		{ print substr($$1, 1, length($$1)-1) }' | 	\
	sort |	\
	pr --omit-pagination --width=80 --columns=1
