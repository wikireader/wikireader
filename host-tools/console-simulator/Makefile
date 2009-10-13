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


# this is broken
# the makefile is completely out of date

RM = rm -f
CC = gcc
LD = ld
GUILIB_DIR = ../../gui-lib
WIKILIB_DIR = ../../wiki-lib
FATFS_DIR = ../../samo-lib/fatfs

CFLAGS = -g -O3 -Wall -MD -D_REENTRANT -Werror
CFLAGS += -I $(WIKILIB_DIR)/include
CFLAGS += -I $(GUILIB_DIR)
CFLAGS += -I $(FATFS_DIR)/src
CFLAGS += -I $(FATFS_DIR)/config/c33/read-write
LDFLAGS = -g -dynamic -lncurses

C_OBJS = guilib.o glyph.o file-io-posix.o wikilib.o time-posix.o
C_OBJS += fontfile.o search.o article.o history.o
C_OBJS += keyboard.o

vpath %.c $(WIKILIB_DIR)/generic $(WIKILIB_DIR)/specific $(GUILIB_DIR) \
	  $(COMMON_DIR)

TARGETS = wikireader
OBJS = $(C_OBJS) main.o

all: $(TARGETS)

wikireader: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean::
	$(RM) $(TARGETS) *.o *.d

-include $(wildcard *.d) dummy

