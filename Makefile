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

DL=./host-tools/toolchain-download
export PATH:=$(shell readlink -m ./host-tools/toolchain-install/bin):$(PATH)

CONFIG_FILE := "samo-lib/include/config.h"
CONFIG_FILE_DEFAULT := "samo-lib/include/config.h-default"
CONFIG_FILE_EXISTS := $(shell [ -f $(CONFIG_FILE) ] && echo 1)

ifeq ($(CONFIG_FILE_EXISTS),)
$(shell cp $(CONFIG_FILE_DEFAULT) $(CONFIG_FILE))
endif

# ----- configuration data --------------------------------------

ALL_TARGETS =
ALL_TARGETS += mbr
ALL_TARGETS += jackknife
ALL_TARGETS += forth
ALL_TARGETS += flash
ALL_TARGETS += mahatma
#ALL_TARGETS += toppers # not working
ALL_TARGETS += qt4-simulator
ALL_TARGETS += wiki-xml
ALL_TARGETS += pcf2bmf

.PHONY: all
all:    ${ALL_TARGETS}

.PHONY: toppers
toppers: mini-libc fatfs
	( cd samo-lib/toppers-jsp && \
	$(MAKE) -C cfg && \
	$(MAKE) -C wikireader && \
	cp wikireader/sample1.elf ../../KERNEL.toppers)

.PHONY: mahatma
mahatma: mini-libc fatfs
	$(MAKE) -C samo-lib/mahatma
	cp -p samo-lib/mahatma/mahatma.elf kernel.elf

# ----- lib stuff   -------------------------------------------
.PHONY:mini-libc
mini-libc: gcc
	$(MAKE) -C samo-lib/mini-libc/

.PHONY: fatfs
fatfs: mini-libc drivers
	$(MAKE) -C samo-lib/fatfs/

.PHONY: drivers
drivers: mini-libc
	$(MAKE) -C samo-lib/drivers/

# ----- toolchain stuff  --------------------------------------
gcc-download:
	mkdir -p $(DL)
	wget -c -O $(DL)/$(GCC_PACKAGE) $(GCC_URL)
	touch $@

binutils-download:
	mkdir -p $(DL)
	wget -c -O $(DL)/$(BINUTILS_PACKAGE) $(BINUTILS_URL)
	touch $@

binutils-patch: binutils-download
	mkdir -p host-tools/toolchain-install
	rm -rf host-tools/binutils-$(BINUTILS_PACKAGE)
	tar -xvzf $(DL)/$(BINUTILS_PACKAGE) -C host-tools/
	( cd host-tools && \
	cd binutils-$(BINUTILS_VERSION) && \
	cat ../toolchain-patches/0001-binutils-EPSON-changes-to-binutils.patch | patch -p1 && \
	cat ../toolchain-patches/0002-binutils-EPSON-make-it-compile-hack-for-recent-gcc.patch | patch -p1)
	touch $@

binutils: binutils-patch
	(cd host-tools && \
	cd binutils-$(BINUTILS_VERSION) && \
	mkdir -p build && \
	cd build  && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" ../configure --prefix $(shell readlink -m ./host-tools/toolchain-install) --target=c33-epson-elf && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" $(MAKE) && \
	$(MAKE) install)
	touch $@

gcc-patch: gcc-download
	mkdir -p host-tools/toolchain-install
	tar -xvzf $(DL)/$(GCC_PACKAGE) -C host-tools/
	( cd host-tools && \
	cd gcc-$(GCC_VERSION) && \
	cat ../toolchain-patches/0001-gcc-EPSON-modified-sources.patch | patch -p1 && \
	cat ../toolchain-patches/0002-gcc-Force-that-the-assembly-of-libgcc-complies-wit.patch | patch -p1 && \
	cat ../toolchain-patches/0003-gcc-Use-the-C-implementations-for-division-and-mod.patch | patch -p1)
	touch $@

gcc: binutils gcc-patch
	( cd host-tools && \
	export PATH=$(shell readlink -m ./host-tools/toolchain-install/bin):$(PATH) && \
	cd gcc-$(GCC_VERSION) && \
	mkdir -p build && \
	cd build && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" ../configure --prefix $(shell readlink -m ./host-tools/toolchain-install) --target=c33-epson-elf --enable-languages=c && \
	CPPFLAGS="-D_FORTIFY_SOURCE=0" $(MAKE) && \
	$(MAKE) install)
	touch $@

.PHONY: qt4-simulator
qt4-simulator: mahatma
	( cd host-tools/qt4-simulator && qmake-qt4 && $(MAKE) )

.PHONY: console-simulator
console-simulator: mahatma
	( cd host-tools/console-simulator && $(MAKE) )

# ----- new wiki rendering  --------------------------------------
.PHONY: wiki-xml
wiki-xml:
	( cd host-tools/wiki-xml && $(MAKE) )

# ----- pcf2bmf  --------------------------------------
.PHONY: pcf2bmf
pcf2bmf:
	( cd host-tools/pcf2bmf && $(MAKE) )

# ----- wiki Dump and Algorithm  --------------------------------------
.PHONY: getwikidump
getwikidump:
	wget http://download.wikimedia.org/enwiki/latest/enwiki-latest-pages-articles.xml.bz2

.PHONY: webkit
webkit:
	svn co -r 41057 http://svn.webkit.org/repository/webkit/trunk webkit
	(cd webkit && \
	patch_path="../host-tools/rendering/patches/"; for file in `ls $$patch_path`; do echo "processing file: $$file"; patch -p1 < $$patch_path/$$file; done && \
	./WebKitTools/Scripts/build-webkit --gtk --release)



# ----- forth -----------------------------------------------
# forth interpreter

.PHONY: forth
forth:  gcc mini-libc fatfs drivers
	$(MAKE) -C samo-lib/forth


# ----- flash -----------------------------------------------
# flash programmer that runs on the device

.PHONY: flash
flash:  gcc mini-libc fatfs drivers
	$(MAKE) -C samo-lib/flash


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

.PHONY: print-mbr-tty
print-mbr-tty:
	@echo BOOTLOADER_TTY = "${BOOTLOADER_TTY}"
	@echo BOOTLOADER_AUX = "${BOOTLOADER_AUX}"

.PHONY: mbr
mbr: gcc fatfs
	$(MAKE) -C samo-lib/mbr

.PHONY: mbr-rs232
mbr-rs232: gcc fatfs
	$(MAKE) -C samo-lib/mbr mbr-rs232

.PHONY: jackknife
jackknife:
	$(MAKE) -C host-tools/jackknife

.PHONY: flash-mbr
flash-mbr: mbr jackknife
	$(MAKE) -C samo-lib/mbr BOOTLOADER_TTY="${BOOTLOADER_TTY}" BOOTLOADER_AUX="${BOOTLOADER_AUX}" SERIAL_NUMBER="${SERIAL_NUMBER}" $@


# ----- clean and help --------------------------------------
.PHONY: complete-clean
complete-clean: clean clean-toolchain
	rm -f binutils-download gcc-download

.PHONY: clean
clean: clean-qt4-simulator clean-console-simulator
	$(MAKE) clean -C samo-lib/mini-libc
	$(MAKE) clean -C host-tools/jackknife
	$(MAKE) clean -C host-tools/wiki-xml
	$(MAKE) clean -C host-tools/pcf2bmf
	$(MAKE) clean -C samo-lib/mbr
	$(MAKE) clean -C samo-lib/drivers
	$(MAKE) clean -C samo-lib/fatfs
	$(MAKE) clean -C samo-lib/forth
	$(MAKE) clean -C samo-lib/flash
	$(MAKE) clean -C samo-lib/mahatma
	cd samo-lib/toppers-jsp && $(MAKE) clean -C wikireader

.PHONY: clean-toolchain
clean-toolchain:
	rm -rf host-tools/toolchain-install
	rm -rf host-tools/gcc-$(GCC_VERSION)
	rm -rf host-tools/binutils-$(BINUTILS_VERSION)
	rm -f binutils-patch binutils
	rm -f gcc-patch gcc

.PHONY: clean-qt4-simulator
clean-qt4-simulator:
	(cd host-tools/qt4-simulator; $(MAKE) distclean || true)

.PHONY: clean-console-simulator
clean-console-simulator:
	$(MAKE) clean -C host-tools/console-simulator

.PHONY:help
help:
	@echo
	@echo 'Some of the more useful targets:'
	@echo
	@echo '  all                   - compile all the source'
	@echo '  mbr                   - compile bootloader'
	@echo '  mahatma               - compile kernel'
	@echo '  forth                 - compile forth'
	@echo '  mbr                   - compile bootloader'
	@echo '  gcc                   - compile gcc toolchain'
	@echo '  flash-mbr             - flash bootloader to the E07 board'
	@echo '  qt4-simulator         - compile the Qt4 simulator'
	@echo '  console-simulator     - compile the console simulator'
	@echo '  clean                 - clean everything except the toochain'
	@echo '  clean-toolchain       - clean just the toochain'
	@echo '  sd                    - copy kernel, forth and programs to SD Card'
	@echo '  p33                   - terminal emulator (console debugging)'
	@echo


.PHONY:testhelp
testhelp:
	$(MAKE) --print-data-base --question |	\
	awk '/^[^.%][-A-Za-z0-9_]*:/		\
		{ print substr($$1, 1, length($$1)-1) }' | 	\
	sort |	\
	pr --omit-pagination --width=80 --columns=1


.PHONY: sd
sd:
	./samo-lib/scripts/MakeSD all

.PHONY: p33
p33:
	./samo-lib/scripts/p33
