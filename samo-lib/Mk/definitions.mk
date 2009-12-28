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


FATFS_MODE     ?= read-only
CROSS_COMPILER ?= YES

# +++START_COPY: this section into the start of each Makefile
#REMARK: Notes:
#REMARK:   1. The "+++TAG:" and "---TAG:" are matched exactly
#REMARK:   2. The prefix "#INC: " will be stripped.  There must be one space
#REMARK:      after the ':' and it is stripped.  Any more spaces are retained.
#REMARK:   3. Lines starting "#REMARK" are not copied.
#REMARK:
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
#INC: include ${MK_DIR}/definitions.mk
# ---END_COPY: of section to copy to individual makefiles


# Standard definitions for _ALL_ Makefiles
# ----------------------------------------

OS := $(strip $(shell uname -s))

export SHELL = /bin/sh
RM = rm -f
MKDIR = mkdir -p
COPY = cp -p
TOUCH = touch

ifeq (Linux,${OS})

LOCKF = flock
AWK = awk

# path must exist
REALPATH = readlink -es

# path may not exist
RESOLVEPATH = readlink -m

else ifeq (FreeBSD,${OS})

LOCKF = lockf
AWK = gawk

# path must exist
REALPATH = realpath

# path may not exist
RESOLVEPATH = /usr/local/bin/readlink -f

else
$(error Unsupported OS: ${OS})
endif


# this is to get a single blank line
define BLANK_LINE


endef

# this allows detection of installed program
# and gives apt-get command to install the missing program
# $1 binary
# $2 list of install names
define REQUIRED_BINARY_1
ifeq (,$$(strip $$(shell which ${1})))
$$(error Error: Could not find program: ${1} \
$${BLANK_LINE}install using:\
$${BLANK_LINE}  sudo apt-get install ${2}\
$${BLANK_LINE}then restart $${MAKE})
endif
endef
REQUIRED_BINARY = $(eval $(call REQUIRED_BINARY_1,$(strip ${1}),$(strip ${2})))


# this allows detection of installed PHP module
# and gives apt-get command to install the missing program
# $1 PHP module name
# $2 list of install names
define REQUIRED_PHP_1
ifneq (YES,$$(strip $$(shell php -r 'echo array_search("$$(strip ${1})", get_loaded_extensions()) ? "YES\n" : "NO\n";')))
$$(error Error: Could not find PHP module: ${1} \
$${BLANK_LINE}install using:\
$${BLANK_LINE}  sudo apt-get install ${2}\
$${BLANK_LINE}then restart $${MAKE})
endif
endef
REQUIRED_PHP = $(eval $(call REQUIRED_PHP_1,$(strip ${1}),$(strip ${2})))


# in the root directory
# =====================

LICENSES := ${ROOT_DIR}/Licenses

WIKI_APP = ${ROOT_DIR}/wiki-app/
WIKI_APP_INCLUDE = ${WIKI_APP}
WIKI_APP_SRC = ${WIKI_APP}


# In samo-lib
# ===========

SAMO_LIB = ${ROOT_DIR}/samo-lib
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

GRIFO = ${SAMO_LIB}/grifo
GRIFO_SCRIPTS = ${GRIFO}/scripts
GRIFO_INCLUDE = ${GRIFO}/include
GRIFO_APPLICATION_LDS = ${GRIFO}/lds/application.lds
GRIFO_LIB = ${GRIFO}/lib/libgrifo.a

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

MISC_FILES := ${SAMO_LIB}/misc-files
SCRIPTS := ${SAMO_LIB}/scripts


# In host-tools
# =============

HOST_TOOLS = ${ROOT_DIR}/host-tools

IMAGE2HEADER= ${HOST_TOOLS}/imagetool/image2header


# Cross compiler definitions
# ==========================

export PATH:=${HOST_TOOLS}/toolchain-install/bin:${PATH}

BUILD_PREFIX =

CROSS = c33-epson-elf-

# substitute cross compiler for mormal utilities
# only if enabled
ifeq (YES,$(strip ${CROSS_COMPILER}))
GCC = $(CROSS)gcc
AS = $(CROSS)as
AR = $(CROSS)ar
LD = $(CROSS)ld
CC = $(GCC)
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump
STRIP = $(CROSS)strip
endif

# for host compile
HOSTCC = gcc
HOSTLD = ld

# for cross
CROSS_GCC = ${CROSS}gcc
CROSS_AS = ${CROSS}as
CROSS_AR = ${CROSS}ar
CROSS_LD = ${CROSS}ld
CROSS_STRIP = ${CROSS}strip
CROSS_OBJDUMP = ${CROSS}objdump
CROSS_OBJCOPY = ${CROSS}objcopy


# Debugging
# =========

ifneq (,$(strip ${DEBUG}))
# special function to display debugging information on every shell
OLD_SHELL := ${SHELL}
SHELL = $(warning [$@ ($^) ($?)])${OLD_SHELL}
endif
