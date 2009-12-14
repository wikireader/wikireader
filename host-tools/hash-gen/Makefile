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


define FIND_SAMO_LIB
d='.' ; \
while [ X"/" != X"$${d}" ] ; \
do \
  s="$${d}/samo-lib" ; \
  [ -d "$${s}" ] && echo $${s} && exit 0 ; \
  d=$$(readlink -es "$${d}/..") ; \
done ; \
echo samo_lib_NOT_FOUND ; \
exit 1
endef
SAMO_LIB := $(shell ${FIND_SAMO_LIB})

include ${SAMO_LIB}/Mk/definitions.mk

CC = gcc
LD = ld

CFLAGS = -g -O3 -Wall -MD -D_REENTRANT -DWIKIPCF -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
CFLAGS += -I. -I$(WIKI_APP_INCLUDE)

LDFLAGS = -g -dynamic


TARGETS = hash-gen

vpath %.c $(WIKI_APP_SRC)

OBJS = main.o
OBJS += hash.o
OBJS += bigram.o
OBJS += search_hash.o


.PHONY: all
all: ${TARGETS}


.PHONY: install
install: all


hash-gen: ${OBJS}
	${CC} ${LDFLAGS} ${OBJS} -o $@

build:
	${MKDIR} "$@"

.PHONY: clean
clean:
	${RM} -r build ${TARGETS} *.o *.d
