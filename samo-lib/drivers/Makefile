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


INCLUDES += -I${DRIVERS_INCLUDE}
INCLUDES += -I${FATFS_INCLUDE} -I${FATFS_CONFIG_INCLUDE}
INCLUDES += -I${MINI_LIBC_INCLUDE}
INCLUDES += -I${SAMO_LIB_INCLUDE}

BUILD_PREFIX := build/

vpath %.c :src

TARGETS = lib/libdrivers.a

# list of library modules
OBJECTS += analog.o
OBJECTS += button.o
OBJECTS += console.o
OBJECTS += ctp.o
OBJECTS += contrast.o
OBJECTS += delay_us.o
OBJECTS += delay_nop.o
OBJECTS += elf32.o
OBJECTS += FLASH.o
OBJECTS += lcd.o
OBJECTS += mmc.o
OBJECTS += print.o
#OBJECTS += sd_spi.o
#OBJECTS += sd_api.o
OBJECTS += SPI.o
OBJECTS += suspend.o
OBJECTS += temperature.o
OBJECTS += tick.o

BUILD_OBJECTS = $(addprefix ${BUILD_PREFIX},${OBJECTS})

.PHONY: all
all:  ${TARGETS}

lib/libdrivers.a: lib build  ${BUILD_OBJECTS}
	${RM} "$@"
	${AR} r "$@" ${BUILD_OBJECTS}

lib:
	${MKDIR} "$@"
build:
	${MKDIR} "$@"

clean:
	$(RM) -r build lib
	${RM} *~ *.o *.d *.lst *.elf *.map *.asm33

include ${MK_DIR}/rules.mk
