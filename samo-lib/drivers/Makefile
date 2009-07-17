# Copyright (c) 2009 Openmoko Inc
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


SAMO_LIB := $(shell readlink -es ../samo-lib || readlink -es ../../samo-lib || readlink -es ../../../samo-lib)

include ${SAMO_LIB}/Mk/definitions.mk


INCLUDES += -I${DRIVERS_INCLUDE}
INCLUDES += -I${FATFS_INCLUDE} -I${FATFS_CONFIG_INCLUDE}
INCLUDES += -I${MINI_LIBC_INCLUDE}
INCLUDES += -I${SAMO_LIB_INCLUDE}
INCLUDES += -I${MAHATMA_INCLUDE}

BUILD_PREFIX := build/

vpath %.c :src

TARGETS = lib/libdrivers.a

# list of library modules
#OBJECTS += serial.o
OBJECTS += sd_spi.o
OBJECTS += sd_api.o
OBJECTS += mmc.o
OBJECTS += tick.o
OBJECTS += contrast.o
OBJECTS += delay_us.o
OBJECTS += delay_nop.o
OBJECTS += lcd.o
OBJECTS += button.o
OBJECTS += ctp.o
OBJECTS += suspend.o

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

include ${SAMO_LIB}/Mk/rules.mk
