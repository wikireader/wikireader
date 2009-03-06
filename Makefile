#
# "WikiReaderMakefile" - a Makefile for setting up Wiki Reader
#
# (C) Copyright 2008, 2009 OpenMoko, Inc.
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

# ----- Toolchain configuration data --------------------------------------

GCC_VERSION=3.3.2
GCC_PACKAGE=gcc-$(GCC_VERSION).tar.gz
GCC_URL=ftp://ftp.gnu.org/gnu/gcc/$(GCC_PACKAGE)

BINUTILS_VERSION=2.10.1
BINUTILS_PACKAGE=binutils-$(BINUTILS_VERSION).tar.gz
BINUTILS_URL= \
  ftp://ftp.gnu.org/gnu/binutils/$(BINUTILS_PACKAGE)

DL=./toolchain/dl
PATCH_BINUTILS=./toolchain/.patch_binutils
PATCH_GCC=./toolchain/.patch_gcc

# ----- configuration data --------------------------------------

.PHONY: all
all:    mini-libc \
	bootloader \
	toppers \
	kernel

.PHONY: bootloader
bootloader:mini-libc fatfs
	( cd bootloader && \
	make )

.PHONY: toppers
toppers: mini-libc fatfs
	( cd jsp && \
	make -C cfg && \
	make -C wikireader && \
	cp wikireader/sample1.elf ../KERNEL.toppers)

.PHONY: kernel
kernel: mini-libc fatfs
	( cd kernel && \
	make && \
	cp mahatma.elf ../KERNEL)

.PHONY: mahatma
mahatma: mini-libc fatfs
	( cd kernel	&& \
	make		&& \
	cp mahatma.elf ../KERNEL)

# ----- lib stuff   -------------------------------------------
.PHONY:mini-libc
mini-libc: toolchain
	make -C toolchain/mini-libc/

.PHONY: fatfs
fatfs: mini-libc
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

$(PATCH_BINUTILS):
	mkdir -p install
	tar -xvzf $(DL)/$(BINUTILS_PACKAGE) -C toolchain/
	( cd toolchain && \
	cd binutils-$(BINUTILS_VERSION) && \
	cat ../patches/0001-binutils-EPSON-changes-to-binutils.patch | patch -p1 && \
	cat ../patches/0002-binutils-EPSON-make-it-compile-hack-for-recent-gcc.patch | patch -p1)
	touch $@

.PHONY: binutils
binutils: $(DL)/$(BINUTILS_PACKAGE).ok $(PATCH_BINUTILS)
	(cd toolchain && \
	cd binutils-$(BINUTILS_VERSION) && \
	mkdir -p build && \
	cd build  && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" ../configure --prefix $(PWD)/install --target=c33-epson-elf && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" make && \
	make install )

$(PATCH_GCC):
	mkdir -p install
	tar -xvzf $(DL)/$(GCC_PACKAGE) -C toolchain/
	( cd toolchain && \
	export PATH=$(PWD)/install/bin:\$(PATH)  && \
	cd gcc-$(GCC_VERSION) && \
	cat ../patches/0001-gcc-EPSON-modified-sources.patch | patch -p1 && \
	cat ../patches/0002-gcc-Force-that-the-assembly-of-libgcc-complies-wit.patch | patch -p1 && \
	cat ../patches/0003-gcc-Use-the-C-implementations-for-division-and-mod.patch | patch -p1)
	touch $@

.PHONY: gcc
gcc: binutils $(DL)/$(GCC_PACKAGE).ok $(PATCH_GCC)
	( cd toolchain && \
	export PATH=$(PWD)/install/bin:\$(PATH)  && \
	cd gcc-$(GCC_VERSION) && \
	mkdir -p build && \
	cd build && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" ../configure --prefix $(PWD)/install --target=c33-epson-elf --enable-languages=c && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" make && \
	make install )

.PHONY: gdb
gdb:


# ----- wiki Dump and Algorithm  --------------------------------------
.PHONY: getwikidump
getwikidump:
	wget http://download.wikimedia.org/enwiki/latest/enwiki-latest-pages-articles.xml.bz2

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
	make clean -C kernel
	cd jsp && make clean -C wikireader
	rm -rf toolchain/gcc-$(GCC_VERSION)
	rm -rf toolchain/binutils-$(BINUTILS_VERSION)
	rm $(PATCH_BINUTILS)	
	rm $(PATCH_GCC)

.PHONY:help
help:
	@echo -e "\n\
all:			compile all the source.\n\
setup:			get all the source we need.\n\
bootloader:		compile bootloader.\n\
toppers:		compile a toppers kernel.\n\
toolchain:		make toolchain-download gcc gdb binutils.\n\
toolchain-download:	downlaod gcc and binutils code we need.\n\
binutils: 		compile binutils.\n\
gcc:			compile gcc.\n\
mini-libc:			compile mini-libc (libc.a).\n\
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
