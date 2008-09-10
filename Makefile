# "WikiReaderMakefile" - a Makefile for setting up Wiki Reader
#
# Copyright (c) 2008  Xiangfu Liu <xiangfu@openmoko.org>
# All rights reserved.
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

WR_SVN_SITE := wikipediardware.googlecode.com
WR_SVN_PATH := svn/trunk
WR_PATH := wikipediardware-read-only

.PHONY: all
all:	setup \
	bootloader \
	wikireader

.PHONY:setup
setup:
	svn checkout http://${MM_SVN_SITE}/${MM_SVN_PATH}/  ${WR_PATH}
	#setup toolchain here

.PHONY: bootloader
bootloader:
	make -C bootloader

.PHONY: wikireader
wikireader:
	(cd jsp && \
	make -C cfg && \
	make -C wikireader && \
	cp wikireader/sample1.elf ../kernel)

#.PHONY: toolchain
#toolchain:
	#download the source, make them setup the env

.PHONY: update
update:update-bootloader update-wikireader

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

.PHONY: clean
clean: 
	make clean -C bootloader
	cd jsp && make clean -C wikireader

.PHONY:help
help:
	@echo "\
all:				compile all the source.\n\
setup:			get all the source we need.\n\
bootloader:		compile bootloader.\n\
wikireader:		compile wikireader then you can see kernel file you need.\n\
update:			both update-bootloader update-wikireader.\n\
update-bootloader: 	update the bootloader source.\n\
update-wikireader: 	update the wikireader source.\n\
check-makefile:		diff the remote makefile.\n\
flash-bootloader: 	flash bootloader to you E07 board\n\
				make sure the serial console is /dev/ttyUSB0.\n\
clean: 			clean all.\n\
					openmoko, Inc.\n "

