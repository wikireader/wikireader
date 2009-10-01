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
#

FATFS_MODE ?= read-only

# autodetect samo-lib
# copy this line into the Makefile
SAMO_LIB := $(shell readlink -es ../samo-lib || readlink -es ../../samo-lib || readlink -es ../../../samo-lib)

# standard definitions for _ALL_ Makefiles

# in samo-lib

SAMO_LIB_MK =  ${SAMO_LIB}/Mk

SAMO_LIB_INCLUDE = ${SAMO_LIB}/include

MINI_LIBC = ${SAMO_LIB}/mini-libc
MINI_LIBC_INCLUDE = ${MINI_LIBC}/include

MINI_LIBC_LIB = ${MINI_LIBC}/lib/libc.a

FATFS = ${SAMO_LIB}/fatfs
FATFS_CONFIG_INCLUDE = ${FATFS}/config/c33/${FATFS_MODE}
FATFS_INCLUDE = ${FATFS}/src
FATFS_SRC = ${FATFS}/src
FATFS_LIB = ${FATFS}/lib/${FATFS_MODE}/libtinyfat.a

DRIVERS = ${SAMO_LIB}/drivers
DRIVERS_INCLUDE = ${DRIVERS}/include
DRIVERS_SRC = ${DRIVERS}/src
DRIVERS_LIB = ${DRIVERS}/lib/libdrivers.a

WIKI_APP = ${SAMO_LIB}/../wiki-app/
WIKI_APP_INCLUDE = ${WIKI_APP}
WIKI_APP_SRC = ${WIKI_APP}

LZMA = ${SAMO_LIB}/lzma
LZMA_INCLUDE = ${LZMA}
LZMA_SRC = ${LZMA}

MAHATMA = ${SAMO_LIB}/mahatma
MAHATMA_INCLUDE = ${MAHATMA}
MAHATMA_SRC = ${MAHATMA}

FORTH = ${SAMO_LIB}/forth
FORTH_INCLUDE = ${FORTH}
FORTH_SRC = ${FORTH}

MBBR = ${SAMO_LIB}/mbr
MBBR_INCLUDE = ${MBBR}
MBBR_SRC = ${MBBR}


# in host-tools

HOST_TOOLS = ${SAMO_LIB}/../host-tools

# cross compiler definitions

export PATH:=${SAMO_LIB}/../host-tools/toolchain-install/bin:${PATH}

BUILD_PREFIX =

CROSS = c33-epson-elf-

GCC = $(CROSS)gcc
AS = $(CROSS)as
AR = $(CROSS)ar
LD = $(CROSS)ld
HOSTCC = gcc
CC = $(GCC)
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump
STRIP = $(CROSS)strip

CROSS_GCC = ${CROSS}gcc
CROSS_AS = ${CROSS}as
CROSS_AR = ${CROSS}ar
CROSS_LD = ${CROSS}ld
CROSS_STRIP = ${CROSS}strip
CROSS_OBJDUMP = ${CROSS}objdump
CROSS_OBJCOPY = ${CROSS}objcopy

RM = rm -f
MKDIR = mkdir -p
COPY = cp -p
TOUCH = touch
