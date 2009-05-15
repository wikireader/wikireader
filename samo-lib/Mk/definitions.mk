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
SAMO_LIB ?= ../samo_lib

# standard definitions for _ALL_ Makefiles

# in samo-lib

SAMO_LIB_MK =  ${SAMO_LIB}/Mk

SAMO_LIB_INCLUDE = ${SAMO_LIB}/include
SAMO_LIB_SRC = ${SAMO_LIB}/src

MINI_LIBC = ${SAMO_LIB}/mini-libc
MINI_LIBC_INCLUDE = ${MINI_LIBC}/include

MINI_LIBC_LIB = ${MINI_LIBC}/lib/libc.a

FATFS = ${SAMO_LIB}/fatfs
FATFS_CONFIG_INCLUDE = ${FATFS}/config/c33/${FATFS_MODE}
FATFS_INCLUDE = ${FATFS}/src
FATFS_SRC = ${FATFS}/src

FATFS_LIB = ${FATFS}/lib/${FATFS_MODE}/libtinyfat.a

WIKI_APP = ${SAMO_LIB}/../wiki-app/
WIKI_APP_INCLUDE = ${WIKI_APP}
WIKI_APP_SRC = ${WIKI_APP}

MINILZO = ${SAMO_LIB}/minilzo
MINILZO_INCLUDE = ${MINILZO}
MINILZO_SRC = ${MINILZO}

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

INDEXING = ${HOST_TOOLS}/indexing/locate
INDEXING_INCLUDE = ${INDEXING}
INDEXING_SRC = ${INDEXING}


# cross compiler definitions

export PATH:=${SAMO_LIB}/../host-tools/toolchain-install/bin:${PATH}

CROSS = c33-epson-elf-

GCC = $(CROSS)gcc
AS = $(CROSS)as
LD = $(CROSS)ld
HOSTCC = gcc
CC = $(GCC)
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump
STRIP = $(CROSS)strip

CROSS_GCC = ${CROSS}gcc
CROSS_AS = ${CROSS}as
CROSS_LD = ${CROSS}ld
CROSS_STRIP = ${CROSS}strip
CROSS_OBJDUMP = ${CROSS}objdump
CROSS_OBJCOPY = ${CROSS}objcopy

RM = rm -f
